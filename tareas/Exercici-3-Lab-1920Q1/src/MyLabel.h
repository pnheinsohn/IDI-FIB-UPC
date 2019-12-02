#include <QLabel>

class MyLabel : public QLabel
{
    Q_OBJECT
    public:
        MyLabel(QWidget *parent);
    
    signals:
        void sendValidation(int valid);

    public slots:
        void setMyText();
        void setValidation(int valid);
        void checkPassword(QString s);
        void checkPasswordConfirmation(QString s);

    private:
        void setColor();

        enum validity {
            VERMELL,
            GROC,
            VERD,
            NUL,
        };

        validity validation;
        QString password;
        QString passwordConfirmation;
};
