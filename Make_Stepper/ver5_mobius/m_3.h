#ifndef m_3_h
#define m_3_h

class m_3
{
    public:
        m_3(int enPin, int stepPin, int dirPin);
        m_3(int enPin, int stepPin, int dirPin, int swich);

        //top motor setup
        void TopSetup();

        void setspeed(int inputSpeed);
        void moveto();
        void moveStep(long steps);
        void setRound(long inputRound);
        void setDiraction();

    private:
        //motor pin
        int enPin;
        int stepPin;
        int dirPin;

        //switch pin
        int swich;

        //motor speed
        int speeds;

        //motor round
        long rounds;

        //motor save steps
        long leftSteps;

        //pre millis
        unsigned long preMillis;

        //motor diraction
        bool diraction;
};

#endif
