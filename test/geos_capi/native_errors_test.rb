# frozen_string_literal: true

require_relative "../test_helper"
require_relative "skip_capi"

class GeosNativeErrorsTest < Minitest::Test
  def setup
    skip "Needs GEOS CAPI." unless RGeo::Geos.capi_supported?
    @factory = RGeo::Geos.factory
  end

  def test_parser_errors_do_not_poison_the_next_call
    100.times do
      error = assert_raises(RGeo::Error::ParseError) { @factory.parse_wkt("POINT (0)") }
      assert_equal "Expected number but encountered ')'", error.message
      assert_equal [1.0, 2.0], @factory.parse_wkt("POINT (1 2)").coordinates
      assert_raises(RGeo::Error::ParseError) { @factory.parse_wkb("\x01\x00") }
    end
  end

  def test_constructor_errors_do_not_poison_the_next_call
    100.times do
      assert_raises(RGeo::Error::InvalidGeometry) { @factory.line_string([@factory.point(0, 0)]) }
      assert_raises(RGeo::Error::InvalidGeometry) do
        @factory.linear_ring([@factory.point(0, 0)])
      end
      assert_equal 2, @factory.line_string([@factory.point(0, 0), @factory.point(1, 1)]).num_points
    end
  end

  def test_error_details_with_colons_are_preserved
    error = assert_raises(RGeo::Error::ParseError) { @factory.parse_wkt("NOT A GEOMETRY") }
    assert_equal "Unknown type: 'NOT'", error.message
  end

  def test_errors_do_not_leak_between_threads
    results = Array.new(4) do |index|
      Thread.new do
        factory = RGeo::Geos.factory
        10.times do
          factory.parse_wkt("INVALID#{index}")
          raise "Expected a native parser error"
        rescue RGeo::Error::ParseError => e
          raise e unless e.message == "Unknown type: 'INVALID#{index}'"
        end
        factory.parse_wkt("POINT (#{index} 2)").coordinates
      end
    end.map(&:value)
    assert_equal [[0.0, 2.0], [1.0, 2.0], [2.0, 2.0], [3.0, 2.0]], results
  end

  def test_overlay_errors_remain_ruby_errors
    invalid = @factory.parse_wkt("POLYGON ((0 0,2 2,0 2,2 0,0 0))")
    valid = @factory.parse_wkt("POLYGON ((0 0,2 0,2 2,0 2,0 0))")
    100.times do
      error = assert_raises(RGeo::Error::GeosError) { invalid.unsafe_intersection(valid) }
      assert_match(/TopologyException/, error.message)
      assert_equal 4.0, valid.intersection(valid).area
    end
  end

  def test_prepared_predicate_errors_are_not_returned_as_truthy_results
    skip "Curved geometries require GEOS 3.13." unless geos_version_match(">= 3.13")
    polygon = @factory.parse_wkt("POLYGON ((0 0,2 0,2 2,0 2,0 0))").prepare!
    curve = @factory.parse_wkt("CIRCULARSTRING (0 0,1 1,2 0)")
    point = @factory.point(1, 1)
    100.times do
      error = assert_raises(RGeo::Error::UnsupportedOperation) { polygon.unsafe_intersects?(curve) }
      assert_equal "Curved geometry types are not supported.", error.message
      assert polygon.intersects?(point)
    end
  end
end
