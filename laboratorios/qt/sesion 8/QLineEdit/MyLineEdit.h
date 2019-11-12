#include <QLineEdit>

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
    public:
        MyLineEdit(QWidget *parent);
    
    public slots:
        void responseReturn();

    signals:
        void sendText(const QString &);
};