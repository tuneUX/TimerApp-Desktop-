#pragma once

#include<QCalendarWidget>
#include<QDate>

class CalendarView : public QCalendarWidget {
	Q_OBJECT

public:
	explicit CalendarView(QWidget* parent = nullptr);

signals:
	void dateSelected(const QDate& date);
	;


private slots:

	void handleDateClicked(const QDate& date);
	

};