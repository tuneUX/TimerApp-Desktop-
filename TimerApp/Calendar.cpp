#include "Calendar.h"

CalendarView::CalendarView(QWidget* parent) : QCalendarWidget(parent) {
    setGridVisible(true);
    setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    connect(this, &QCalendarWidget::clicked, this, &CalendarView::handleDateClicked);
}

void CalendarView::handleDateClicked(const QDate& date) {
    emit dateSelected(date);
}
