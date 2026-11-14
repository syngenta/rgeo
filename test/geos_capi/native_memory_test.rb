# frozen_string_literal: true

require_relative "../test_helper"
require_relative "skip_capi"
require "objspace"

class GeosNativeMemoryTest < Minitest::Test
  def setup
    skip "Needs GEOS CAPI." unless RGeo::Geos.capi_supported?
    @factory = RGeo::Geos.factory
    @polygon = @factory.parse_wkt("POLYGON((0 0,10 0,10 10,5 12,0 10,0 0))")
    @line = @factory.parse_wkt("LINESTRING(-1 5,11 5)")
  end

  def test_geometry_storage_is_reported
    coordinates = Array.new(1000) { |i| "#{i} #{i % 3}" }.join(",")
    large = @factory.parse_wkt("LINESTRING(#{coordinates})")
    assert_operator ObjectSpace.memsize_of(large), :>=, 1000 * 16
    assert_operator ObjectSpace.memsize_of(large), :>, ObjectSpace.memsize_of(@line)
  end

  def test_explicit_preparation_is_counted_once
    # Exclude Ruby's lazily allocated validity-cache storage from this comparison.
    @polygon.check_validity!
    original_size = ObjectSpace.memsize_of(@polygon)
    @polygon.prepare!
    prepared_size = ObjectSpace.memsize_of(@polygon)
    assert_operator prepared_size, :>, original_size
    3.times do
      @polygon.prepare!
      assert @polygon.intersects?(@line)
    end
    assert_equal prepared_size, ObjectSpace.memsize_of(@polygon)
  end

  def test_automatic_preparation_is_reported
    original_size = ObjectSpace.memsize_of(@polygon)
    assert @polygon.intersects?(@line)
    refute @polygon.prepared?
    assert @polygon.intersects?(@line)
    assert @polygon.prepared?
    assert_operator ObjectSpace.memsize_of(@polygon), :>, original_size
  end

  def test_native_allocations_apply_gc_pressure
    wkb = large_line_wkb
    without_gc do
      before = GC.stat(:malloc_increase_bytes)
      geometry = @factory.parse_wkb(wkb)
      allocated = GC.stat(:malloc_increase_bytes) - before
      assert_operator allocated, :>=, 2048 * 16
      assert_equal 2048, geometry.num_points
    end
  end

  def test_preparation_applies_gc_pressure
    geometry = @factory.parse_wkb(large_line_wkb)
    without_gc do
      before = GC.stat(:malloc_increase_bytes)
      geometry.prepare!
      allocated = GC.stat(:malloc_increase_bytes) - before
      assert_operator allocated, :>=, 2048 * 16
    end
  end

  def test_cross_factory_lines_keep_cast_points_alive_during_gc
    other_factory = RGeo::Geos.factory(srid: 4326)
    start_point = other_factory.point(1, 2)
    end_point = other_factory.point(3, 4)
    previous_stress = GC.stress
    GC.stress = true
    5.times do
      line = @factory.line(start_point, end_point)
      assert_equal [[1.0, 2.0], [3.0, 4.0]], line.coordinates
    end
  ensure
    GC.stress = previous_stress
  end

  def test_automatic_preparation_keeps_cross_factory_arguments_alive
    other_factory = RGeo::Geos.factory(srid: 4326)
    point = other_factory.point(1, 1)
    predicates = {
      intersects?: true, contains?: true, disjoint?: false, touches?: false,
      crosses?: false, within?: false, overlaps?: false
    }
    previous_stress = GC.stress
    GC.stress = true
    predicates.each do |predicate, expected|
      polygon = @polygon.dup
      2.times { assert_equal expected, polygon.public_send(predicate, point) }
      assert polygon.prepared?
    end
  ensure
    GC.stress = previous_stress
  end

  def test_preparation_is_counted_even_when_the_predicate_raises
    skip "Curved geometries require GEOS 3.13." unless geos_version_match(">= 3.13")
    curve = @factory.parse_wkt("CIRCULARSTRING (0 0,1 1,2 0)")
    before = ObjectSpace.memsize_of(@polygon)
    2.times do
      assert_raises(RGeo::Error::UnsupportedOperation) { @polygon.unsafe_intersects?(curve) }
    end
    assert @polygon.prepared?
    assert_operator ObjectSpace.memsize_of(@polygon), :>, before
  end

  def test_nested_three_dimensional_components_are_counted
    factory = RGeo::Geos.factory(has_z_coordinate: true)
    polygon = factory.parse_wkt(
      "POLYGON Z ((0 0 1,10 0 1,10 10 1,0 10 1,0 0 1)," \
      "(2 2 1,2 3 1,3 3 1,3 2 1,2 2 1))"
    )
    collection = factory.collection([factory.collection([polygon]), polygon])
    assert_operator ObjectSpace.memsize_of(polygon), :>=, 10 * 24
    assert_operator ObjectSpace.memsize_of(collection), :>=, 20 * 24
    restored = Marshal.load(Marshal.dump(collection))
    assert_equal collection, restored
    assert_equal ObjectSpace.memsize_of(collection), ObjectSpace.memsize_of(restored)
  end

  def test_copy_marshal_and_collection_keep_independent_ownership
    original_size = ObjectSpace.memsize_of(@polygon)
    @polygon.prepare!
    copy = @polygon.dup
    refute copy.prepared?
    assert_equal original_size, ObjectSpace.memsize_of(copy)
    restored = Marshal.load(Marshal.dump(@polygon))
    refute restored.prepared?
    assert_equal original_size, ObjectSpace.memsize_of(restored)
    collection = @factory.collection([copy, restored])
    assert_operator ObjectSpace.memsize_of(collection), :>=, 2 * (original_size - 40)
    3.times { GC.start(full_mark: true, immediate_sweep: true) }
    assert collection.intersects?(@line)
    assert @polygon.intersects?(@line)
  end

  private

  def large_line_wkb
    [1, 2, 2048].pack("CL<L<") +
      Array.new(2048) { |i| [i.to_f, (i % 2).to_f] }.flatten.pack("E*")
  end

  def without_gc
    previously_disabled = GC.disable
    yield
  ensure
    GC.enable unless previously_disabled
  end
end
