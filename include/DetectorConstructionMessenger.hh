#ifndef DETECTOR_CONSTRUCTOR_MESSENGER_HH
#define DETECTOR_CONSTRUCTOR_MESSENGER_HH

#include "G4UImessenger.hh"

namespace DETECTOR
{
    class Detector;
    class Manager;

    // This clas manages the macro file that is used to control the detector geometry
    class Messenger : public G4UImessenger
    {
        public:
            Messenger( Manager* manager );
            ~Messenger();

            void SetNewValue( G4UIcommand* cmd, G4String value ) override;

        private:
            Manager* m_detector_manager;
            G4UIdirectory* m_cmd_directory;
            G4UIcommand* m_cmd_change_thickness;
            G4UIcommand* m_cmd_change_z;
    };
}
#endif
