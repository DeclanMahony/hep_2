
#include "DataManager.C"
#include "SpacePointConverter.C"
#include "StreamIO.C"


void draw(std::string dirname="data/")
{

  // ----------------------------------------------------------------------
  // instantiate the class that handles all the data access
  // auto dman = DataManager("./");
  auto dman = DataManager(dirname);
  //auto dman = DataManager("/alice/share/simdata/");

  // ----------------------------------------------------------------------
  // create output objects
  TH2F* padrow = new TH2F("padrow", "padrow",144,0.,144.,30,0.,30.);
  TCanvas* cnv = new TCanvas("cnv_padrow", "cnv_padrow", 800,600);

  // TFile* outfile=new TFile("outfile.root", "RECREATE");

  SpacePointConverter conv;

  // ----------------------------------------------------------------------
  // loop over time frames and events
  while ( dman.NextTimeFrame() ) {
    while ( dman.NextEvent() ) {

      for (auto& seq : dman.DigitsByPadRow()) {

        padrow->Reset();
        padrow->SetStats(0);
        padrow->GetXaxis()->SetRange(seq.begin()->getPadCol(),
        (seq.end()-1)->getPadCol()+2);


        int det = seq.begin()->getDetector();
        int row = seq.begin()->getPadRow();

        cout << (*seq.begin()) << " digits: "
             << (seq.end()-seq.begin()) << endl;

        for (auto& dig : seq) {
          cout << "   " << dig << endl;

          auto adc = dig.getADC();
          for (int i=0;i<30;i++) {
            padrow->Fill(dig.getPadCol(), i, adc[i]);
          }
        }

        // new TCanvas();
        // auto p = padrow->Clone(Form("padrow_%03d_%02d",det,row));
        //p->Draw("colz");
        // p->Write();


        padrow->SetTitle(Form("Det %03d row %02d;pad;time bin",det,row));
        padrow->Draw("colz");
        padrow->Draw("text,same");


        TMarker m;
        m.SetMarkerStyle(20);

        for (auto& hit : dman.Hits()) {

          // only use hits in current detector
          if (hit.GetDetectorID()!=det) continue;

          // convert xyz to pad row/col/timebin coordinates
          auto rct = conv.Hit2RowColTime(hit);

          // restrict to current padrow
          if (rct[0]<float(row) || rct[0]>float(row+1)) continue;

          cout << hit << "   " << rct[0] << ":" << rct[1] << ":" << rct[2] << endl;
          m.DrawMarker(rct[1], rct[2]);
        }

        cnv->SaveAs(Form("padrow_%03d_%02d.pdf",det,row));

      } // padrow loop


      return;

    } // event/trigger record loop
  } // time frame loop

}
