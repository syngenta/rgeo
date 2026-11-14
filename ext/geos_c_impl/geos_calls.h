/*
 * GEOS may report an error from inside a C++ catch block. Raise only after
 * the C API returns, so that its exception object and message are released.
 * Cleanup functions stay raw; polygon/collection constructors are checked
 * by their callers after releasing their temporary pointer arrays.
 */
#ifndef RGEO_GEOS_CALLS_INCLUDED
#define RGEO_GEOS_CALLS_INCLUDED

#include "preface.h"
#include <geos_c.h>

#include "errors.h"

#ifdef RGEO_GEOS_SUPPORTED
RGEO_BEGIN_C

#define RGEO_GEOS_RESULT(type, name, parameters, arguments)                    \
  static inline type rgeo_##name parameters                                    \
  {                                                                            \
    type result = name arguments;                                              \
    rgeo_raise_geos_error();                                                   \
    return result;                                                             \
  }

#define RGEO_GEOS_PREDICATE(name, parameters, arguments)                       \
  static inline char rgeo_##name parameters                                    \
  {                                                                            \
    char result = name arguments;                                              \
    rgeo_raise_geos_error();                                                   \
    if (result == 2)                                                           \
      rb_raise(rb_eGeosError, #name " failed");                                \
    return result;                                                             \
  }

#define RGEO_GEOS_VOID(name, parameters, arguments)                            \
  static inline void rgeo_##name parameters                                    \
  {                                                                            \
    name arguments;                                                            \
    rgeo_raise_geos_error();                                                   \
  }

RGEO_GEOS_RESULT(int,
                 GEOSArea,
                 (const GEOSGeometry* g, double* area),
                 (g, area))
RGEO_GEOS_RESULT(GEOSGeometry*, GEOSBoundary, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSBuffer,
                 (const GEOSGeometry* g, double width, int quadsegs),
                 (g, width, quadsegs))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSBufferWithStyle,
                 (const GEOSGeometry* g,
                  double width,
                  int quadsegs,
                  int endCapStyle,
                  int joinStyle,
                  double mitreLimit),
                 (g, width, quadsegs, endCapStyle, joinStyle, mitreLimit))
RGEO_GEOS_PREDICATE(GEOSContains,
                    (const GEOSGeometry* g1, const GEOSGeometry* g2),
                    (g1, g2))
RGEO_GEOS_RESULT(GEOSGeometry*, GEOSConvexHull, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(GEOSCoordSequence*,
                 GEOSCoordSeq_clone,
                 (const GEOSCoordSequence* s),
                 (s))
RGEO_GEOS_RESULT(GEOSCoordSequence*,
                 GEOSCoordSeq_create,
                 (unsigned int size, unsigned int dims),
                 (size, dims))
RGEO_GEOS_RESULT(int,
                 GEOSCoordSeq_getSize,
                 (const GEOSCoordSequence* s, unsigned int* size),
                 (s, size))
RGEO_GEOS_RESULT(int,
                 GEOSCoordSeq_getX,
                 (const GEOSCoordSequence* s, unsigned int idx, double* val),
                 (s, idx, val))
RGEO_GEOS_RESULT(int,
                 GEOSCoordSeq_getY,
                 (const GEOSCoordSequence* s, unsigned int idx, double* val),
                 (s, idx, val))
RGEO_GEOS_RESULT(int,
                 GEOSCoordSeq_getZ,
                 (const GEOSCoordSequence* s, unsigned int idx, double* val),
                 (s, idx, val))
#ifdef RGEO_GEOS_SUPPORTS_ISCCW
RGEO_GEOS_RESULT(int,
                 GEOSCoordSeq_isCCW,
                 (const GEOSCoordSequence* s, char* is_ccw),
                 (s, is_ccw))
#endif
RGEO_GEOS_RESULT(int,
                 GEOSCoordSeq_setX,
                 (GEOSCoordSequence * s, unsigned int idx, double val),
                 (s, idx, val))
RGEO_GEOS_RESULT(int,
                 GEOSCoordSeq_setY,
                 (GEOSCoordSequence * s, unsigned int idx, double val),
                 (s, idx, val))
RGEO_GEOS_RESULT(int,
                 GEOSCoordSeq_setZ,
                 (GEOSCoordSequence * s, unsigned int idx, double val),
                 (s, idx, val))
RGEO_GEOS_PREDICATE(GEOSCrosses,
                    (const GEOSGeometry* g1, const GEOSGeometry* g2),
                    (g1, g2))
#ifdef RGEO_GEOS_SUPPORTS_DENSIFY
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSDensify,
                 (const GEOSGeometry* g, double tolerance),
                 (g, tolerance))
#endif
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSDifference,
                 (const GEOSGeometry* ga, const GEOSGeometry* gb),
                 (ga, gb))
RGEO_GEOS_PREDICATE(GEOSDisjoint,
                    (const GEOSGeometry* g1, const GEOSGeometry* g2),
                    (g1, g2))
RGEO_GEOS_RESULT(int,
                 GEOSDistance,
                 (const GEOSGeometry* g1, const GEOSGeometry* g2, double* dist),
                 (g1, g2, dist))
RGEO_GEOS_RESULT(GEOSGeometry*, GEOSEnvelope, (const GEOSGeometry* g), (g))
RGEO_GEOS_PREDICATE(GEOSEquals,
                    (const GEOSGeometry* g1, const GEOSGeometry* g2),
                    (g1, g2))
RGEO_GEOS_PREDICATE(GEOSEqualsExact,
                    (const GEOSGeometry* g1,
                     const GEOSGeometry* g2,
                     double tolerance),
                    (g1, g2, tolerance))
RGEO_GEOS_RESULT(int, GEOSGeomTypeId, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(GEOSGeometry*, GEOSGeom_clone, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSGeom_createLineString,
                 (GEOSCoordSequence * s),
                 (s))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSGeom_createLinearRing,
                 (GEOSCoordSequence * s),
                 (s))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSGeom_createPoint,
                 (GEOSCoordSequence * s),
                 (s))
RGEO_GEOS_RESULT(const GEOSCoordSequence*,
                 GEOSGeom_getCoordSeq,
                 (const GEOSGeometry* g),
                 (g))
RGEO_GEOS_RESULT(GEOSGeometry*, GEOSGetCentroid, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(const GEOSGeometry*,
                 GEOSGetExteriorRing,
                 (const GEOSGeometry* g),
                 (g))
RGEO_GEOS_RESULT(const GEOSGeometry*,
                 GEOSGetGeometryN,
                 (const GEOSGeometry* g, int n),
                 (g, n))
RGEO_GEOS_RESULT(const GEOSGeometry*,
                 GEOSGetInteriorRingN,
                 (const GEOSGeometry* g, int n),
                 (g, n))
RGEO_GEOS_RESULT(int, GEOSGetNumCoordinates, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(int, GEOSGetNumGeometries, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(int, GEOSGetNumInteriorRings, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(int, GEOSGetSRID, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSInterpolate,
                 (const GEOSGeometry* line, double d),
                 (line, d))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSIntersection,
                 (const GEOSGeometry* g1, const GEOSGeometry* g2),
                 (g1, g2))
RGEO_GEOS_PREDICATE(GEOSIntersects,
                    (const GEOSGeometry* g1, const GEOSGeometry* g2),
                    (g1, g2))
RGEO_GEOS_RESULT(int,
                 GEOSLength,
                 (const GEOSGeometry* g, double* length),
                 (g, length))
RGEO_GEOS_RESULT(GEOSGeometry*, GEOSMakeValid, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(GEOSGeometry*, GEOSNode, (const GEOSGeometry* g), (g))
RGEO_GEOS_PREDICATE(GEOSOverlaps,
                    (const GEOSGeometry* g1, const GEOSGeometry* g2),
                    (g1, g2))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSPointOnSurface,
                 (const GEOSGeometry* g),
                 (g))
#ifdef RGEO_GEOS_SUPPORTS_POLYGON_HULL_SIMPLIFY
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSPolygonHullSimplify,
                 (const GEOSGeometry* g,
                  unsigned int isOuter,
                  double vertexNumFraction),
                 (g, isOuter, vertexNumFraction))
#endif
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSPolygonize,
                 (const GEOSGeometry* const geoms[], unsigned int ngeoms),
                 (geoms, ngeoms))
RGEO_GEOS_RESULT(const GEOSPreparedGeometry*,
                 GEOSPrepare,
                 (const GEOSGeometry* g),
                 (g))
#ifdef RGEO_GEOS_SUPPORTS_PREPARED1
RGEO_GEOS_PREDICATE(GEOSPreparedContains,
                    (const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2),
                    (pg1, g2))
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
RGEO_GEOS_PREDICATE(GEOSPreparedCrosses,
                    (const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2),
                    (pg1, g2))
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
RGEO_GEOS_PREDICATE(GEOSPreparedDisjoint,
                    (const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2),
                    (pg1, g2))
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED1
RGEO_GEOS_PREDICATE(GEOSPreparedIntersects,
                    (const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2),
                    (pg1, g2))
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
RGEO_GEOS_PREDICATE(GEOSPreparedOverlaps,
                    (const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2),
                    (pg1, g2))
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
RGEO_GEOS_PREDICATE(GEOSPreparedTouches,
                    (const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2),
                    (pg1, g2))
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
RGEO_GEOS_PREDICATE(GEOSPreparedWithin,
                    (const GEOSPreparedGeometry* pg1, const GEOSGeometry* g2),
                    (pg1, g2))
#endif
RGEO_GEOS_RESULT(double,
                 GEOSProject,
                 (const GEOSGeometry* line, const GEOSGeometry* point),
                 (line, point))
RGEO_GEOS_PREDICATE(GEOSRelatePattern,
                    (const GEOSGeometry* g1,
                     const GEOSGeometry* g2,
                     const char* imPattern),
                    (g1, g2, imPattern))
RGEO_GEOS_VOID(GEOSSetSRID, (GEOSGeometry * g, int SRID), (g, SRID))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSSimplify,
                 (const GEOSGeometry* g, double tolerance),
                 (g, tolerance))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSSymDifference,
                 (const GEOSGeometry* ga, const GEOSGeometry* gb),
                 (ga, gb))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSTopologyPreserveSimplify,
                 (const GEOSGeometry* g, double tolerance),
                 (g, tolerance))
RGEO_GEOS_PREDICATE(GEOSTouches,
                    (const GEOSGeometry* g1, const GEOSGeometry* g2),
                    (g1, g2))
#ifdef RGEO_GEOS_SUPPORTS_UNARYUNION
RGEO_GEOS_RESULT(GEOSGeometry*, GEOSUnaryUnion, (const GEOSGeometry* g), (g))
#endif
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSUnion,
                 (const GEOSGeometry* ga, const GEOSGeometry* gb),
                 (ga, gb))
RGEO_GEOS_RESULT(GEOSWKBReader*, GEOSWKBReader_create, (void), ())
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSWKBReader_read,
                 (GEOSWKBReader * reader,
                  const unsigned char* wkb,
                  size_t size),
                 (reader, wkb, size))
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSWKBReader_readHEX,
                 (GEOSWKBReader * reader,
                  const unsigned char* hex,
                  size_t size),
                 (reader, hex, size))
RGEO_GEOS_RESULT(GEOSWKBWriter*, GEOSWKBWriter_create, (void), ())
RGEO_GEOS_VOID(GEOSWKBWriter_setOutputDimension,
               (GEOSWKBWriter * writer, int newDimension),
               (writer, newDimension))
RGEO_GEOS_RESULT(unsigned char*,
                 GEOSWKBWriter_write,
                 (GEOSWKBWriter * writer, const GEOSGeometry* g, size_t* size),
                 (writer, g, size))
RGEO_GEOS_RESULT(GEOSWKTReader*, GEOSWKTReader_create, (void), ())
RGEO_GEOS_RESULT(GEOSGeometry*,
                 GEOSWKTReader_read,
                 (GEOSWKTReader * reader, const char* wkt),
                 (reader, wkt))
RGEO_GEOS_RESULT(GEOSWKTWriter*, GEOSWKTWriter_create, (void), ())
RGEO_GEOS_VOID(GEOSWKTWriter_setOutputDimension,
               (GEOSWKTWriter * writer, int dim),
               (writer, dim))
RGEO_GEOS_VOID(GEOSWKTWriter_setTrim,
               (GEOSWKTWriter * writer, char trim),
               (writer, trim))
RGEO_GEOS_RESULT(char*,
                 GEOSWKTWriter_write,
                 (GEOSWKTWriter * writer, const GEOSGeometry* g),
                 (writer, g))
RGEO_GEOS_PREDICATE(GEOSWithin,
                    (const GEOSGeometry* g1, const GEOSGeometry* g2),
                    (g1, g2))
RGEO_GEOS_PREDICATE(GEOSisEmpty, (const GEOSGeometry* g), (g))
RGEO_GEOS_PREDICATE(GEOSisRing, (const GEOSGeometry* g), (g))
RGEO_GEOS_PREDICATE(GEOSisSimple, (const GEOSGeometry* g), (g))
RGEO_GEOS_PREDICATE(GEOSisValid, (const GEOSGeometry* g), (g))
RGEO_GEOS_RESULT(
  char,
  GEOSisValidDetail,
  (const GEOSGeometry* g, int flags, char** reason, GEOSGeometry** location),
  (g, flags, reason, location))

#undef RGEO_GEOS_RESULT
#undef RGEO_GEOS_PREDICATE
#undef RGEO_GEOS_VOID

#define GEOSArea rgeo_GEOSArea
#define GEOSBoundary rgeo_GEOSBoundary
#define GEOSBuffer rgeo_GEOSBuffer
#define GEOSBufferWithStyle rgeo_GEOSBufferWithStyle
#define GEOSContains rgeo_GEOSContains
#define GEOSConvexHull rgeo_GEOSConvexHull
#define GEOSCoordSeq_clone rgeo_GEOSCoordSeq_clone
#define GEOSCoordSeq_create rgeo_GEOSCoordSeq_create
#define GEOSCoordSeq_getSize rgeo_GEOSCoordSeq_getSize
#define GEOSCoordSeq_getX rgeo_GEOSCoordSeq_getX
#define GEOSCoordSeq_getY rgeo_GEOSCoordSeq_getY
#define GEOSCoordSeq_getZ rgeo_GEOSCoordSeq_getZ
#ifdef RGEO_GEOS_SUPPORTS_ISCCW
#define GEOSCoordSeq_isCCW rgeo_GEOSCoordSeq_isCCW
#endif
#define GEOSCoordSeq_setX rgeo_GEOSCoordSeq_setX
#define GEOSCoordSeq_setY rgeo_GEOSCoordSeq_setY
#define GEOSCoordSeq_setZ rgeo_GEOSCoordSeq_setZ
#define GEOSCrosses rgeo_GEOSCrosses
#ifdef RGEO_GEOS_SUPPORTS_DENSIFY
#define GEOSDensify rgeo_GEOSDensify
#endif
#define GEOSDifference rgeo_GEOSDifference
#define GEOSDisjoint rgeo_GEOSDisjoint
#define GEOSDistance rgeo_GEOSDistance
#define GEOSEnvelope rgeo_GEOSEnvelope
#define GEOSEquals rgeo_GEOSEquals
#define GEOSEqualsExact rgeo_GEOSEqualsExact
#define GEOSGeomTypeId rgeo_GEOSGeomTypeId
#define GEOSGeom_clone rgeo_GEOSGeom_clone
#define GEOSGeom_createLineString rgeo_GEOSGeom_createLineString
#define GEOSGeom_createLinearRing rgeo_GEOSGeom_createLinearRing
#define GEOSGeom_createPoint rgeo_GEOSGeom_createPoint
#define GEOSGeom_getCoordSeq rgeo_GEOSGeom_getCoordSeq
#define GEOSGetCentroid rgeo_GEOSGetCentroid
#define GEOSGetExteriorRing rgeo_GEOSGetExteriorRing
#define GEOSGetGeometryN rgeo_GEOSGetGeometryN
#define GEOSGetInteriorRingN rgeo_GEOSGetInteriorRingN
#define GEOSGetNumCoordinates rgeo_GEOSGetNumCoordinates
#define GEOSGetNumGeometries rgeo_GEOSGetNumGeometries
#define GEOSGetNumInteriorRings rgeo_GEOSGetNumInteriorRings
#define GEOSGetSRID rgeo_GEOSGetSRID
#define GEOSInterpolate rgeo_GEOSInterpolate
#define GEOSIntersection rgeo_GEOSIntersection
#define GEOSIntersects rgeo_GEOSIntersects
#define GEOSLength rgeo_GEOSLength
#define GEOSMakeValid rgeo_GEOSMakeValid
#define GEOSNode rgeo_GEOSNode
#define GEOSOverlaps rgeo_GEOSOverlaps
#define GEOSPointOnSurface rgeo_GEOSPointOnSurface
#ifdef RGEO_GEOS_SUPPORTS_POLYGON_HULL_SIMPLIFY
#define GEOSPolygonHullSimplify rgeo_GEOSPolygonHullSimplify
#endif
#define GEOSPolygonize rgeo_GEOSPolygonize
#define GEOSPrepare rgeo_GEOSPrepare
#ifdef RGEO_GEOS_SUPPORTS_PREPARED1
#define GEOSPreparedContains rgeo_GEOSPreparedContains
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
#define GEOSPreparedCrosses rgeo_GEOSPreparedCrosses
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
#define GEOSPreparedDisjoint rgeo_GEOSPreparedDisjoint
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED1
#define GEOSPreparedIntersects rgeo_GEOSPreparedIntersects
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
#define GEOSPreparedOverlaps rgeo_GEOSPreparedOverlaps
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
#define GEOSPreparedTouches rgeo_GEOSPreparedTouches
#endif
#ifdef RGEO_GEOS_SUPPORTS_PREPARED2
#define GEOSPreparedWithin rgeo_GEOSPreparedWithin
#endif
#define GEOSProject rgeo_GEOSProject
#define GEOSRelatePattern rgeo_GEOSRelatePattern
#define GEOSSetSRID rgeo_GEOSSetSRID
#define GEOSSimplify rgeo_GEOSSimplify
#define GEOSSymDifference rgeo_GEOSSymDifference
#define GEOSTopologyPreserveSimplify rgeo_GEOSTopologyPreserveSimplify
#define GEOSTouches rgeo_GEOSTouches
#ifdef RGEO_GEOS_SUPPORTS_UNARYUNION
#define GEOSUnaryUnion rgeo_GEOSUnaryUnion
#endif
#define GEOSUnion rgeo_GEOSUnion
#define GEOSWKBReader_create rgeo_GEOSWKBReader_create
#define GEOSWKBReader_read rgeo_GEOSWKBReader_read
#define GEOSWKBReader_readHEX rgeo_GEOSWKBReader_readHEX
#define GEOSWKBWriter_create rgeo_GEOSWKBWriter_create
#define GEOSWKBWriter_setOutputDimension rgeo_GEOSWKBWriter_setOutputDimension
#define GEOSWKBWriter_write rgeo_GEOSWKBWriter_write
#define GEOSWKTReader_create rgeo_GEOSWKTReader_create
#define GEOSWKTReader_read rgeo_GEOSWKTReader_read
#define GEOSWKTWriter_create rgeo_GEOSWKTWriter_create
#define GEOSWKTWriter_setOutputDimension rgeo_GEOSWKTWriter_setOutputDimension
#define GEOSWKTWriter_setTrim rgeo_GEOSWKTWriter_setTrim
#define GEOSWKTWriter_write rgeo_GEOSWKTWriter_write
#define GEOSWithin rgeo_GEOSWithin
#define GEOSisEmpty rgeo_GEOSisEmpty
#define GEOSisRing rgeo_GEOSisRing
#define GEOSisSimple rgeo_GEOSisSimple
#define GEOSisValid rgeo_GEOSisValid
#define GEOSisValidDetail rgeo_GEOSisValidDetail

RGEO_END_C
#endif
#endif
