#include <QLabel>
// #include <sstream>
// #include <string>


class MyLabel : public QLabel
{
    Q_OBJECT
    public:
        MyLabel(QWidget *parent);
    
    public slots:
        void setRedColor(int value);
        void setGreenColor(int value);
        void setBlueColor(int value);

    private:
        int redColor;
        int greenColor;
        int blueColor;
};
