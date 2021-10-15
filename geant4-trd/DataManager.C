
// #include <>

#include <TTreeReaderArray.h>
#include <TTreeReader.h>
#include <TFile.h>

#include <DataFormatsTRD/TriggerRecord.h>
#include <DataFormatsTRD/Digit.h>
#include <DataFormatsTRD/Hit.h>


int DigitIdx(const o2::trd::Digit& a)
{
  int ret = a.getDetector();
  ret *= 1000; ret += a.getPadRow();
  ret *= 1000; ret += a.getPadCol();
  return ret;
}

bool DigitCompare(const o2::trd::Digit& a, const o2::trd::Digit& b)
{
  return DigitIdx(a) < DigitIdx(b);
}

class DataManager
{

private:
  TFile *fhits, *fdigits;
  TTree *trhits, *trdigits, *trtrgrec;
  TTreeReader *rdrhits, *rdrdigits, *rdrtrgreg;

  TTreeReaderArray<o2::trd::Hit>* hits;
  TTreeReaderArray<o2::trd::Digit>* digits;
  TTreeReaderArray<o2::trd::TriggerRecord>* trgrecords;

  int tfno, evno;
  size_t iEvent;


public:

  DataManager(std::string dir="data/");

  bool NextTimeFrame();
  bool NextEvent();


  o2::trd::TriggerRecord GetTriggerRecord() {return trgrecords->At(iEvent);}

  // A struct to be used for range-based for loops
  // A template would be nice, but I did not manage to make it work.
  struct DigitRange {
    TTreeReaderArray<o2::trd::Digit>::iterator& begin() { return b; }
    TTreeReaderArray<o2::trd::Digit>::iterator& end() { return e; }
    TTreeReaderArray<o2::trd::Digit>::iterator b, e;
   };

   DigitRange event_digits;//, padrow_digits;
   DigitRange Digits() {return event_digits;}
   list<DataManager::DigitRange> DigitsByPadRow();

  // A struct to be used for range-based for loops
  // A template would be nice, but I did not manage to make it work.
  struct HitRange {
    TTreeReaderArray<o2::trd::Hit>::iterator& begin() { return b; }
    TTreeReaderArray<o2::trd::Hit>::iterator& end() { return e; }
    TTreeReaderArray<o2::trd::Hit>::iterator b, e;
   };

   HitRange event_hits;
   HitRange Hits() {return event_hits;}


protected:
  void ConnectMCHitsFile(std::string fname)
  {
    // ----------------------------------------------------------------------
    // set up data structures for reading

    if (fhits || trhits) {
      cerr << "Hits file seems to be connected." << endl;
      return;
    }

    fhits = new TFile(fname.c_str());
    fhits->GetObject("o2sim", trhits);

    rdrhits = new TTreeReader(trhits);
    hits = new TTreeReaderArray<o2::trd::Hit>(*rdrhits, "TRDHit");
  }

  void ConnectDigitsFile(std::string fname)
  {

    fdigits = new TFile(fname.c_str());
    fdigits->GetObject("o2sim", trdigits);

    rdrdigits = new TTreeReader(trdigits);

    // set up the branches we want to read
    digits = new TTreeReaderArray<o2::trd::Digit>(*rdrdigits, "TRDDigit");
    trgrecords = new TTreeReaderArray<o2::trd::TriggerRecord>(*rdrdigits, "TriggerRecord");
  }


};

DataManager::DataManager(std::string dir)
: fhits(0), fdigits(0), trhits(0), trdigits(0), rdrhits(0), rdrdigits(0),
  hits(0), digits(0), trgrecords(0),
  tfno(0), evno(0), iEvent(-1)
{
  ConnectMCHitsFile(dir+"o2sim_HitsTRD.root");
  ConnectDigitsFile(dir+"trddigits.root");
}

bool DataManager::NextTimeFrame()
{
  if (rdrdigits->Next() ) {

    cout << "## Time frame " << tfno << endl;

    iEvent = 0;
    evno = 0;
    tfno++;
    return true;

  } else {
    return false;
  }

}

bool DataManager::NextEvent()
{
  // get the next trigger record
  if (iEvent >= trgrecords->GetSize()) {
    return false;
  }

  // load the hits for the next event
  if ( ! rdrhits->Next() ) {
    cout << "no hits found for event " << tfno << ":" << evno << endl;
    return false;
  }

  auto evrec = GetTriggerRecord();
  cout << evrec << endl;

  cout << "## Event " << tfno << ":" << evno << ":  "
       << hits->GetSize() << " hits   "
       << evrec.getNumberOfDigits() << " digits" << endl;

  event_digits.b = digits->begin() + evrec.getFirstDigit();
  event_digits.e = event_digits.begin() + evrec.getNumberOfDigits();
  std::stable_sort(event_digits.b,event_digits.e,DigitCompare);

  event_hits.b  = hits->begin();
  event_hits.e  = hits->end();

  evno++;
  iEvent++;
  return true;
}

list<DataManager::DigitRange> DataManager::DigitsByPadRow()
{
  list<DataManager::DigitRange> ranges;

  DataManager::DigitRange rng;
  rng.b = event_digits.b;
  rng.e = event_digits.b;

  while ( rng.e != event_digits.e ) {

    if ( rng.b->getDetector() != rng.e->getDetector() ||
         rng.b->getPadRow() != rng.e->getPadRow() ) {

      ranges.push_back(rng);
      rng.b = rng.e;
    }

    rng.e++;
  }

  ranges.push_back(rng);
  return ranges;
}
