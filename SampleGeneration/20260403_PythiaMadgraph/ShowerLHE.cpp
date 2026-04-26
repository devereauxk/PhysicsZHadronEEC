#include <iostream>
#include <memory>
using namespace std;

#include "CommandLine.h"

#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC3.h"
#include "HepMC3/WriterAscii.h"

using namespace Pythia8;

int main(int argc, char *argv[])
{
   CommandLine CL(argc, argv);

   string InputFileName = CL.Get("Input");
   string OutputFileName = CL.Get("Output");
   string ConfigFileName = CL.Get("Config", "CP5.cmnd");
   int EventCount = CL.GetInteger("Events", -1);
   int Seed = CL.GetInteger("Seed", 12345);
   bool Quiet = CL.GetBool("Quiet", true);

   Pythia Pythia;

   if(ConfigFileName != "")
      Pythia.readFile(ConfigFileName);

   if(Quiet == true)
   {
      Pythia.readString("Init:showChangedSettings = off");
      Pythia.readString("Init:showChangedParticleData = off");
      Pythia.readString("Next:numberShowInfo = 0");
      Pythia.readString("Next:numberShowProcess = 0");
      Pythia.readString("Next:numberShowEvent = 0");
      Pythia.readString("Print:quiet = on");
   }

   Pythia.readString("Random:setSeed = on");
   Pythia.readString("Random:seed = " + to_string(Seed));
   Pythia.readString("Beams:frameType = 4");
   Pythia.readString("Beams:LHEF = " + InputFileName);
   if(EventCount > 0)
      Pythia.readString("Main:numberOfEvents = " + to_string(EventCount));

   Pythia.init();

   HepMC3::Pythia8ToHepMC3 ToHepMC;
   ToHepMC.set_store_pdf(false);
   ToHepMC.set_store_proc(false);
   ToHepMC.set_print_inconsistency(false);
   ToHepMC.set_free_parton_warnings(false);

   HepMC3::WriterAscii Writer(OutputFileName);

   int Accepted = 0;
   while(true)
   {
      if(EventCount > 0 && Accepted >= EventCount)
         break;

      if(Pythia.next() == false)
      {
         if(Pythia.info.atEndOfFile() == true)
            break;
         continue;
      }

      HepMC3::GenEvent Event;
      ToHepMC.fill_next_event(Pythia, Event);
      Event.weights().clear();
      Event.weights().push_back(Pythia.info.weight());
      Event.set_event_number(Accepted + 1);
      Writer.write_event(Event);

      Accepted = Accepted + 1;
   }

   Writer.close();
   Pythia.stat();

   cout << "Showered " << Accepted << " events into " << OutputFileName << endl;

   return 0;
}
