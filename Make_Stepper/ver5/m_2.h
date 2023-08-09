#ifndef m_2_h
#define m_2_h

class m_2
{
    public:
        m_2(int enPin, int stepPin, int dirPin);
        m_2(int enPin, int stepPin, int dirPin, int swich);

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
