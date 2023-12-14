// class Mapper {
//   mapper: any;
//
//   constructor(gj?: FeatureCollection) {
//     this.mapper = new NMapper(gj);
//
//     console.log(this.mapper);
//   }
//
//     addGeoJson(gj: FeatureCollection) {
//         this.mapper.addGeoJson(gj);
//     }
//
//   findRoutes(coordinates: [number, number][]): [number, number][] | null {
//     let path: [number, number][] | null = null;
//     try {
//       path = this.mapper.findRoutes(coordinates);
//     } catch (err) {
//       console.error(err);
//     } finally {
//       return path;
//     }
//   }
// }
//

import { featureCollection } from "@turf/helpers";

// export default Mapper;
export default class Mapper {
  constructor(gj?: object);
  findRoutes(coordinates: [number, number][]): [number, number][] | null;
  addGeoJson(gj: object): void;
}
