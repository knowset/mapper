const { Mapper: NMapper } = require("./build/Release/Mapper");

// test
class Mapper {
  mapper: any;

  constructor(gj?: object) {
    this.mapper = new NMapper(gj);
    console.log(this.mapper);
  }

  findRoutes(coordinates: [number, number][]): [number, number][] {
    return this.mapper.findRoutes(coordinates);
  }
}

export default Mapper;
