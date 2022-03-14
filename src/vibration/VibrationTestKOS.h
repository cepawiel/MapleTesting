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


class VibrationMenuKOS : public GenericMenu, public RefCnt {
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

        void AddtoScene(RefPtr<Scene> scene) { 
            UpdateText();
            return scene->subAdd(m_Label); 
        }

        void Select() { m_Label->setTint(m_Selected); }
        void Deselect() { m_Label->setTint(m_Deselected); }
        virtual void Left() = 0;
        virtual void Right() = 0;

        virtual void UpdateText() = 0;    

    };

    class IntSettingRow : public SettingRow {
    protected:
        int m_Val = 0;
        int m_Min = INT32_MIN;
        int m_Max = INT32_MAX;
    public:
        IntSettingRow(Font * fnt, std::string label, float x, float y) : 
            SettingRow(fnt, label, x, y) {

        }
        ~IntSettingRow() {};

        virtual void UpdateText() override {
            if(m_Val > m_Max) m_Val = m_Min;
            if(m_Val < m_Min) m_Val = m_Max;
            sprintf(&m_Buffer[0], "%s < %d >\0", m_LabelName.c_str(), m_Val);
            //sprintf(&m_Buffer[0], "ASDF\0");
        }

        void SetMin(int min) { m_Min = min; }
        void SetMax(int max) { m_Max = max; }

        virtual void Left() override { m_Val--; UpdateText(); }
        virtual void Right() override { m_Val++; UpdateText(); }

        void SetInt(int i) { m_Val = i; }
        int GetInt() { return m_Val; }

    };

    class BoolSettingRow : public SettingRow {
    protected:
        bool m_Val = false;
    public:
        BoolSettingRow(Font * fnt, std::string label, float x, float y) : 
            SettingRow(fnt, label, x, y) {

        }
        ~BoolSettingRow() {};

        virtual void UpdateText() override {
            if (m_Val) {
                sprintf(&m_Buffer[0], "%s %s\0", m_LabelName.c_str(), "< true >");
            } else {
                sprintf(&m_Buffer[0], "%s %s\0", m_LabelName.c_str(), "< false >");
            }
            
        }

        virtual void Left() override { m_Val = !m_Val; UpdateText(); }
        virtual void Right() override { m_Val = !m_Val; UpdateText(); }

        void SetBool(int i) { m_Val = i; }
        bool GetBool() { return m_Val; }

    };

    purupuru_effect_t m_VibrationValues;

    static const int NUMBER_OF_ROWS = 11;
    const char * title = "Motor Testing";
    const char * controls = "Run (A)\tCancel (B)\tExit (Start)";
    uint8_t m_RowPos = 0;
    std::shared_ptr<SettingRow> m_Rows[NUMBER_OF_ROWS] = {};

    std::shared_ptr<IntSettingRow> runTimer;
    std::shared_ptr<BoolSettingRow> motor1;
    std::shared_ptr<BoolSettingRow> motor2;
    std::shared_ptr<BoolSettingRow> specialPulse;
    std::shared_ptr<BoolSettingRow> e1Pulse;
    std::shared_ptr<BoolSettingRow> e1PowerSave;
    std::shared_ptr<IntSettingRow> effect1;
    std::shared_ptr<BoolSettingRow> e2Pulse;
    std::shared_ptr<BoolSettingRow> e2Decay;
    std::shared_ptr<IntSettingRow> effect2UIntensity;
    std::shared_ptr<IntSettingRow> effect2LIntensity;

    RefPtr<Label> titleLabel, controlsLabel;
public:
    VibrationMenuKOS(Font * fnt) {
        
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

        runTimer = std::make_shared<IntSettingRow>(fnt, "Run Timer", xStart, yStart + pos * ySpacing);
        runTimer->SetInt(20);
        runTimer->SetMin(0);
        runTimer->SetMax(255);
        runTimer->Select();
        runTimer->AddtoScene(m_scene);
        m_Rows[pos++] = runTimer;

        motor1 = std::make_shared<BoolSettingRow>(fnt, "Motor 1 En", xStart, yStart + pos * ySpacing);
        motor1->SetBool(true);
        motor1->AddtoScene(m_scene);
        m_Rows[pos++] = motor1;

        motor2 = std::make_shared<BoolSettingRow>(fnt, "Motor 2 En", xStart, yStart + pos * ySpacing);
        motor2->AddtoScene(m_scene);
        m_Rows[pos++] = motor2;

        specialPulse = std::make_shared<BoolSettingRow>(fnt, "Special Pulse", xStart, yStart + pos * ySpacing);
        specialPulse->AddtoScene(m_scene);
        m_Rows[pos++] = specialPulse;

        e1Pulse = std::make_shared<BoolSettingRow>(fnt, "Effect 1 Pulse", xStart, yStart + pos * ySpacing);
        e1Pulse->AddtoScene(m_scene);
        m_Rows[pos++] = e1Pulse;

        e1PowerSave = std::make_shared<BoolSettingRow>(fnt, "Effect 1 Power Save", xStart, yStart + pos * ySpacing);
        e1PowerSave->AddtoScene(m_scene);
        m_Rows[pos++] = e1PowerSave;

        effect1 = std::make_shared<IntSettingRow>(fnt, "Effect 1", xStart, yStart + pos * ySpacing);
        effect1->SetInt(1);
        effect1->SetMin(0);
        effect1->SetMax(7);
        effect1->AddtoScene(m_scene);
        m_Rows[pos++] = effect1;

        e2Pulse = std::make_shared<BoolSettingRow>(fnt, "Effect 2 Pulse", xStart, yStart + pos * ySpacing);
        e2Pulse->AddtoScene(m_scene);
        m_Rows[pos++] = e2Pulse;

        e2Decay = std::make_shared<BoolSettingRow>(fnt, "Effect 2 Decay", xStart, yStart + pos * ySpacing);
        e2Decay->AddtoScene(m_scene);
        m_Rows[pos++] = e2Decay;

        effect2UIntensity = std::make_shared<IntSettingRow>(fnt, "Effect 2 UIntensity", xStart, yStart + pos * ySpacing);
        effect2UIntensity->SetInt(1);
        effect2UIntensity->SetMin(0);
        effect2UIntensity->SetMax(7);
        effect2UIntensity->AddtoScene(m_scene);
        m_Rows[pos++] = effect2UIntensity;

        effect2LIntensity = std::make_shared<IntSettingRow>(fnt, "Effect 2 LIntensity", xStart, yStart + pos * ySpacing);
        effect2LIntensity->SetInt(1);
        effect2LIntensity->SetMin(0);
        effect2LIntensity->SetMax(7);
        effect2LIntensity->AddtoScene(m_scene);
        m_Rows[pos++] = effect2LIntensity;

    }

    virtual ~VibrationMenuKOS() {
    }

    virtual void inputEvent(const Event & evt) {
        if(evt.type != Event::EvtKeypress)
            return;


        maple_device_t * puru = maple_enum_type(evt.port, MAPLE_FUNC_PURUPURU);

        switch(evt.key) {
            case Event::KeyUp:
                m_Rows[m_RowPos]->Deselect();
                m_RowPos--;
                if(m_RowPos < 0) m_RowPos = NUMBER_OF_ROWS - 1;
                m_Rows[m_RowPos]->Select();
                break;
            case Event::KeyDown:
                m_Rows[m_RowPos]->Deselect();
                m_RowPos++;
                if(m_RowPos >= NUMBER_OF_ROWS) m_RowPos = 0;
                m_Rows[m_RowPos]->Select();
                break;
            case Event::KeyLeft:
                m_Rows[m_RowPos]->Left();
                break;
            case Event::KeyRight:
                m_Rows[m_RowPos]->Right();
                break;
            case Event::KeyStart:
                startExit();
                break;
            case Event::KeySelect:
                m_VibrationValues.duration = runTimer->GetInt();
                m_VibrationValues.effect1 = PURUPURU_EFFECT1_INTENSITY(effect1->GetInt());
                if(e1Pulse->GetBool()) m_VibrationValues.effect1 |= PURUPURU_EFFECT1_PULSE;
                if(e1PowerSave->GetBool()) m_VibrationValues.effect1 |= PURUPURU_EFFECT1_POWERSAVE;
                m_VibrationValues.effect2 = PURUPURU_EFFECT2_LINTENSITY(effect2LIntensity->GetInt()) | PURUPURU_EFFECT2_UINTENSITY(effect2UIntensity->GetInt());
                if(e2Decay->GetBool()) m_VibrationValues.effect2 |= PURUPURU_EFFECT2_DECAY;
                if(e2Pulse->GetBool()) m_VibrationValues.effect2 |= PURUPURU_EFFECT2_PULSE;
                m_VibrationValues.special = 0;
                if(motor1->GetBool()) m_VibrationValues.special |= PURUPURU_SPECIAL_MOTOR1;
                if(motor2->GetBool()) m_VibrationValues.special |= PURUPURU_SPECIAL_MOTOR2;
                if(specialPulse->GetBool()) m_VibrationValues.special |= PURUPURU_SPECIAL_PULSE;
                if(puru != nullptr)
                    purupuru_rumble(puru, &m_VibrationValues);
                break;
            case Event::KeyCancel:
                m_VibrationValues.duration = 0;
                m_VibrationValues.effect1 = 0;
                m_VibrationValues.effect2 = 0;
                m_VibrationValues.special = 0;
                if(puru != nullptr)
                    purupuru_rumble(puru, &m_VibrationValues);
                break;
        }
    }

    virtual void startExit() {
        GenericMenu::startExit();
    }
};