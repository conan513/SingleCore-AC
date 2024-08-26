const fs = require("fs");
const { parse } = require("csv-parse");

if (process.argv.length !== 3) {
  console.error('You must specify a filename for importing');
  process.exit(1);
}

let filename = process.argv[2];


let counter = 0;
fs.createReadStream(filename)
  .pipe(parse({ delimiter: ",", from_line: 2 }))
  .on("data", function(row) {
    if (counter === 0) {
      outputHeader();
    }

    if (counter !== 0) {
      console.log(",");
    }
    console.log(
      "(",
      row[1], ",",
      row[2], ",",
      row[3], ",",
      row[4], ",",
      row[5], ",",
      "'", row[0], "')"
    );
    counter++;
    // if (counter !== 100) {
    //   console.log(",");
    // }

    if (counter === 100) {
      console.log(";");
      counter = 0;
    }
  })
  .on("end", function() {
    console.log(";");
   });

function outputHeader() {

  console.log("INSERT INTO mod_auctionator_market_price ");
  console.log("(`entry`, `average_price`, `buyout`, `bid`, `count`, `scan_datetime`) VALUES");

}
