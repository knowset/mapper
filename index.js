"use strict";
const { Mapper: NMapper } = require("./build/Release/Mapper");



// test
class Mapper {
  mapper;
  constructor(gj) {
    this.mapper = new NMapper(gj);

    console.log(this.mapper);
  }

  addGeoJson(gj) {
    this.mapper.addGeoJson(gj);
  }

  findRoutes(coordinates) {
    let path = null;
    try {
      path = this.mapper.findRoutes(coordinates);
    } catch (err) {
      console.error(err);
    } finally {
      return path;
    }
  }
}

module.exports = Mapper;

