
std::ostream& operator<<(std::ostream& os, const o2::trd::Hit& h)
{
  os << h.GetDetectorID() << ":"
     << h.getLocalC() << "/" << h.getLocalT() << "/" << h.getLocalR()
     << "  q=" << h.GetCharge();

  return os;
}
