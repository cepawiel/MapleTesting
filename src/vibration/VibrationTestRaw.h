#pragma once

#include <kos.h>
#include <string>
#include <memory>
#include <math.h>
#include <tsu/genmenu.h>
#include <tsu/font.h>

#include <tsu/drawables/label.h>
#include <tsu/anims/logxymover.h>
#include <tsu/anims/expxymover.h>
#include <tsu/anims/alphafader.h>
#include <tsu/triggers/death.h>



class VibrationMenuRaw : public GenericMenu, public RefCnt {
protected:
    class SettingRow {
    protected:
        Color m_Selected;
        Color m_Deselected;

        RefPtr<Label> m_Label;
        std::string m_LabelName;
        char m_Buffer[32] = {};
    public:
        SettingRow(Font * fnt, std::string label, float x, float y) : 
            m_LabelName(label),
            m_Selected(1, 1, 1, 1),
            m_Deselected(1, 0.7f, 0.7f, 0.7f) {

            sprintf(&m_Buffer[0], "asdf\0");

            m_Label = new Label(fnt, m_Buffer, 24, false, true);
            
            m_Label->setTranslate(Vector(x, y, 0));
            m_Label->setTint(m_Deselected);
        }
        ~SettingRow() {};

        virtual void AddtoScene(RefPtr<Scene> scene) { 
            UpdateText();
            return scene->subAdd(m_Label); 
        }

        void Select() { m_Label->setTint(m_Selected); }
        void Deselect() { m_Label->setTint(m_Deselected); }
        virtual void Left() = 0;
        virtual void Right() = 0;
        virtual void Up() = 0;
        virtual void Down() = 0;

        virtual void UpdateText() = 0;    

    };

    // class IntSettingRow : public SettingRow {
    // protected:
    //     int m_Val = 0;
    //     int m_Min = INT32_MIN;
    //     int m_Max = INT32_MAX;
    // public:
    //     IntSettingRow(Font * fnt, std::string label, float x, float y) : 
    //         SettingRow(fnt, label, x, y) {

    //     }
    //     ~IntSettingRow() {};

    //     virtual void UpdateText() override {
    //         if(m_Val > m_Max) m_Val = m_Min;
    //         if(m_Val < m_Min) m_Val = m_Max;
    //         sprintf(&m_Buffer[0], "%s < %d >\0", m_LabelName.c_str(), m_Val);
    //         //sprintf(&m_Buffer[0], "ASDF\0");
    //     }

    //     void SetMin(int min) { m_Min = min; }
    //     void SetMax(int max) { m_Max = max; }

    //     virtual void Left() override { m_Val--; UpdateText(); }
    //     virtual void Right() override { m_Val++; UpdateText(); }

    //     void SetInt(int i) { m_Val = i; }
    //     int GetInt() { return m_Val; }

    // };

    class HexSettingCol : public SettingRow {
    protected:
        int m_Val = 0;
        int m_Min = INT32_MIN;
        int m_Max = INT32_MAX;
    public:
        HexSettingCol(Font * fnt, float x, float y) : 
            SettingRow(fnt, "", x, y) {

        }
        ~HexSettingCol() {};

        virtual void UpdateText() override {
            if(m_Val > m_Max) m_Val = m_Min;
            if(m_Val < m_Min) m_Val = m_Max;
            sprintf(&m_Buffer[0], "^\n%X\nV\0", m_Val);
        }

        void SetMin(int min) { m_Min = min; }
        void SetMax(int max) { m_Max = max; }

        virtual void Up() override { m_Val++; UpdateText(); }
        virtual void Down() override { m_Val--; UpdateText(); }
        virtual void Left() override {}
        virtual void Right() override {}

        void SetInt(int i) { m_Val = i; }
        int GetInt() { return m_Val; }

    };

    class HexSettingRow : public SettingRow {
    protected:
        static const uint8_t NIBBLE_COUNT = 8;
        uint8_t selectedNibble = 0;
        std::shared_ptr<HexSettingCol> nibbleSetting[NIBBLE_COUNT] = {};
    public:
        HexSettingRow(Font * fnt, std::string label, float x, float y, int initialValue = 0) : 
            SettingRow(fnt, label, x, y) {
            int shiftSize = 28;
            for(int i = 0; i  < NIBBLE_COUNT; i++) {
                nibbleSetting[i] = std::make_shared<HexSettingCol>(fnt, x + i * 32, y + 32);
                
                uint8_t nibble = (initialValue >> shiftSize) & 0xF;
                printf("%X", nibble);
                nibbleSetting[i]->SetInt(nibble);

                nibbleSetting[i]->SetMin(0);
                nibbleSetting[i]->SetMax(15);

                shiftSize -= 4;
            }
            nibbleSetting[selectedNibble]->Select();
            printf("\n");
        }
        ~HexSettingRow() {};

        virtual void UpdateText() override {
            sprintf(&m_Buffer[0], "%s\0", m_LabelName.c_str());
            for(int i = 0; i  < NIBBLE_COUNT; i++)
                nibbleSetting[i]->UpdateText();
        }

        virtual void AddtoScene(RefPtr<Scene> scene) {
            for(int i = 0; i  < NIBBLE_COUNT; i++)
                nibbleSetting[i]->AddtoScene(scene);
            return SettingRow::AddtoScene(scene);
        }

        virtual void Left() override { 
            nibbleSetting[selectedNibble]->Deselect();
            if(selectedNibble == 0){
                selectedNibble = NIBBLE_COUNT - 1;
            } else {
                selectedNibble--;
            }
            nibbleSetting[selectedNibble]->Select();
        }
        virtual void Right() override {
            nibbleSetting[selectedNibble]->Deselect();
            if(selectedNibble == NIBBLE_COUNT - 1){
                selectedNibble = 0;
            } else {
                selectedNibble++;
            }
            nibbleSetting[selectedNibble]->Select();
        }
        virtual void Up() override { 
            nibbleSetting[selectedNibble]->Up();
        }
        virtual void Down() override {
            nibbleSetting[selectedNibble]->Down();
        }

        void SetInt(int i) {  }
        int GetInt() { 
            uint32_t val = 0;
            for(int i = 0; i  < NIBBLE_COUNT; i++) {
                uint32_t nib = nibbleSetting[i]->GetInt();
                val |= (nib << (((NIBBLE_COUNT - 1) - i) * 4));
            }
            //printf("0x%X\n", val);
            return val; 
        }

    };

    // class BoolSettingRow : public SettingRow {
    // protected:
    //     bool m_Val = false;
    // public:
    //     BoolSettingRow(Font * fnt, std::string label, float x, float y) : 
    //         SettingRow(fnt, label, x, y) {

    //     }
    //     ~BoolSettingRow() {};

    //     virtual void UpdateText() override {
    //         if (m_Val) {
    //             sprintf(&m_Buffer[0], "%s %s\0", m_LabelName.c_str(), "< true >");
    //         } else {
    //             sprintf(&m_Buffer[0], "%s %s\0", m_LabelName.c_str(), "< false >");
    //         }
            
    //     }

    //     virtual void Left() override { m_Val = !m_Val; UpdateText(); }
    //     virtual void Right() override { m_Val = !m_Val; UpdateText(); }

    //     void SetBool(int i) { m_Val = i; }
    //     bool GetBool() { return m_Val; }

    // };

    uint32_t m_MotorData = 0;


    //static const int NUMBER_OF_ROWS = 11;
    const char * title = "Raw Motor Testing";
    const char * controls = "Run (A)\tCancel (B)\tExit (Start)";
    // uint8_t m_RowPos = 0;
    // std::shared_ptr<SettingRow> m_Rows[NUMBER_OF_ROWS] = {};

    std::shared_ptr<HexSettingRow> hexValue;

    RefPtr<Label> titleLabel, controlsLabel;
public:
    VibrationMenuRaw(Font * fnt) {
        
        // Offset our scene so 0,0,0 is the screen center with Z +10
        m_scene->setTranslate(Vector(0, 0, 10));

        // Set a green background
        setBg(0.2f, 0.4f, 0.2f);

        Color white(1, 1, 1, 1);

        
        titleLabel = new Label(fnt, title, 34, false, true);
        titleLabel->setTranslate(Vector(30, 30, 0));
        titleLabel->setTint(white);
        m_scene->subAdd(titleLabel);

        controlsLabel = new Label(fnt, controls, 24, true, true);
        controlsLabel->setTranslate(Vector(320, 480 - 24, 0));
        controlsLabel->setTint(white);
        m_scene->subAdd(controlsLabel);

        const int ySpacing = 24;
        const int yStart = 70;
        const int xStart = 40;
        int pos = 0;

        hexValue = std::make_shared<HexSettingRow>(fnt, "Raw Value", xStart, yStart + pos * ySpacing, 0x14111010);
        hexValue->Select();
        hexValue->AddtoScene(m_scene);

    }

    virtual ~VibrationMenuRaw() {
    }

    void rumbleAll(uint32_t val) {
        for(int i = 0; i < 4; i++) {
            maple_device_t * puru = maple_enum_type(i, MAPLE_FUNC_PURUPURU);
            if(puru != nullptr) {
                printf("Sending 0x%X to Port %d\n", val, puru->port);
                purupuru_rumble_raw(puru, val);
            }
        }
    }

    virtual void inputEvent(const Event & evt) {
        if(evt.type != Event::EvtKeypress)
            return;
    
        switch(evt.key) {
            case Event::KeyUp:
                hexValue->Up();
                break;
            case Event::KeyDown:
                hexValue->Down();
                break;
            case Event::KeyLeft:
                hexValue->Left();
                break;
            case Event::KeyRight:
                hexValue->Right();
                break;
            case Event::KeyStart:
                startExit();
                break;
            case Event::KeySelect:
                rumbleAll(hexValue->GetInt());
                // if(puru != nullptr)
                //     purupuru_rumble_raw(puru, val);
                break;
            case Event::KeyCancel:
                // purupuru_effect_t m_VibrationValues = {};
                // m_VibrationValues.duration = 0;
                // m_VibrationValues.effect1 = 0;
                // m_VibrationValues.effect2 = 0;
                // m_VibrationValues.special = 0;
                rumbleAll(0);
                // if(puru != nullptr)
                //     purupuru_rumble(puru, &m_VibrationValues);
                break;
        }
    }

    virtual void startExit() {
        GenericMenu::startExit();
    }
};
