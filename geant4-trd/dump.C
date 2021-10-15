
#include "DataManager.C"
#include "SpacePointConverter.C"
#include "StreamIO.C"

#include <iomanip>

void dump(std::string dirname = "data/") {

  // ----------------------------------------------------------------------
  // instantiate the class that handles all the data access
  auto dman = DataManager(dirname);
  // auto dman = DataManager("/alice/share/simdata/");

  SpacePointConverter conv;

  int event = 0;

  ofstream digitsfile("digits.csv");
  digitsfile << "event,det,row,col" << setfill('0');
  for (int i = 0; i < 30; i++) {
    digitsfile << ",a" << setw(2) << i;
  }
  digitsfile << setfill(' ') << endl;

  // ----------------------------------------------------------------------
  // loop over time frames and events
  while (dman.NextTimeFrame()) {
    while (dman.NextEvent()) {

      event++;

      for (auto &digit : dman.Digits()) {

        digitsfile << event << "," << digit.getDetector() << ","
                   << digit.getPadRow() << "," << digit.getPadCol();

        auto adc = digit.getADC();
        for (int i = 0; i < 30; i++) {
          digitsfile << "," << adc[i];
        }
        digitsfile << endl;
      }

      // for (auto &hit : dman.Hits()) {
      //
      //   // only use hits in current detector
      //   if (hit.GetDetectorID() != det)
      //     continue;
      //
      //   // convert xyz to pad row/col/timebin coordinates
      //   auto rct = conv.Hit2RowColTime(hit);
      //
      //   // restrict to current padrow
      //   if (rct[0] < float(row) || rct[0] > float(row + 1))
      //     continue;
      //
      //   cout << hit << "   " << rct[0] << ":" << rct[1] << ":" << rct[2]
      //        << endl;
      //   m.DrawMarker(rct[1], rct[2]);
      // }

      // cnv->SaveAs(Form("padrow_%03d_%02d.pdf", det, row));

      // } // padrow loop

      // return;

    } // event/trigger record loop
  } // time frame loop
}
