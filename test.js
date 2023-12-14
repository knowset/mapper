const Mapper = require(".");
const gj = require("./highway-line.json");

const m = new Mapper(gj);

console.log(
  m.findRoutes([[
    36.9869031,
    60.0259717,
  ], [
    37.0447695,
    60.0208196,
  ]]),
);

// POINTS:  [
//   [ 59.21987993062571, 39.94251251220703 ],
//   [ 59.21416940981837, 39.778060913085945 ]
// ]
