//
// Created by XMuli <xmulitech@gmail.com> on 2022/01/16.
//
#ifndef XTEXTWIDGET_H
#define XTEXTWIDGET_H

#include <QTextEdit>

class XTextWidget : public QTextEdit
{
    Q_OBJECT
public:
	explicit XTextWidget(QWidget *parent = nullptr);
	~XTextWidget();

	void adjustSize();
	void setFont(const QFont& font);

protected:
	void showEvent(QShowEvent *e) override;
	void resizeEvent(QResizeEvent *e) override;

signals:
    void textUpdated(const QString& s);

public /*slots*/:
    void setTextColor(const QColor& c);
    void setAlignment(Qt::AlignmentFlag alignment);

private slots:
	void emitTextUpdated();

private:
	QSize m_baseSize;
	QSize m_minSize;
};

#endif // XTEXTWIDGET_H
