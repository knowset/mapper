// Geometry Types: Point, LineString, Polygon, MultiPoint, MultiLineString, MultiPolygon, and GeometryCollection;

export declare type GeometryTypes = "Point" | "MultiPoint" | "LineString";

export declare type Properties = {
    [name: string]: any;
} | null;

/*
 *   Position
 *
 *   A position is an array of numbers.  There MUST be two or more
 *   elements.  The first two elements are longitude and latitude, or
 *   easting and northing, precisely in that order and using decimal
 *   numbers.  Altitude or elevation MAY be included as an optional third
 *   element.
 */
export type Position = number[];

export type GeoJsonObject = {
    // A GeoJSON object represents a Geometry, Feature, or collection of
    // Features.
    //
    // o  A GeoJSON object is a JSON object.
    //
    // o  A GeoJSON object has a member with the name "type".  The value of
    //    the member MUST be one of the GeoJSON types.
    type: string;
    // o  A GeoJSON object MAY have a "bbox" member, the value of which MUST
    //    be a bounding box array https://datatracker.ietf.org/doc/html/rfc7946#section-5.
    //
    // o  A GeoJSON object MAY have other members https://datatracker.ietf.org/doc/html/rfc7946#section-6

    bbox?: any;
    // other members ? https://datatracker.ietf.org/doc/html/rfc7946#section-6
};

export type Point = Pick<Geometry, "coordinates"> & {
    type: "Point";
};

export type MultiPoint = Pick<Geometry, "coordinates"> & {
    type: "MultiPoint";
};

export type LineString = Pick<Geometry, "coordinates"> & {
    type: "LineString";
};

// FIXME:   add types for other objects.

export type Geometry = {
    type: GeometryTypes;
    coordinates: Position | Position[];
} & GeoJsonObject;

// export type GeometryObject = {
//     // A Geometry object represents points, curves, and surfaces in
//     // coordinate space.  Every Geometry object is a GeoJSON object no
//     // matter where it occurs in a GeoJSON text.
//     //
//     // o  The value of a Geometry object's "type" member MUST be one of the
//     //    seven geometry types https://datatracker.ietf.org/doc/html/rfc7946#section-1.4.
//     type: GeometryTypes;
//     // o  A GeoJSON Geometry object of any type other than
//     //    "GeometryCollection" has a member with the name "coordinates".
//     //    The value of the "coordinates" member is an array.  The structure
//     //    of the elements in this array is determined by the type of
//     //    geometry.  GeoJSON processors MAY interpret Geometry objects with
//     //    empty "coordinates" arrays as null objects.
//     coordinates: Position | Position[];
// } & GeoJsonObject;

export type GeometryCollection = {
    type: "GeometryCollection";
    geometries: Geometry[];
};

export type Feature<G = Geometry | GeometryCollection, P = Properties> = {
    type: "Feature";
    geometry: G;
    properties: P;
};

export type FeatureCollection<
    G = Geometry | GeometryCollection,
    P = Properties
> = {
    type: "FeatureCollection";
    features: Feature<G, P>[];
} & GeoJsonObject;

