#pragma once
#include <algorithm>
#include <utility>
#include <random>
#include <QString>

// Base interface for line sorting.
class ISorter
{
private:
	bool _isDescending = true;
	size_t _fromColumn = 0;
	size_t _toColumn = 0;

protected:
	bool isDescending() const
	{
		return _isDescending;
	}

	QString getSortKey(const QString& input)
	{
		if (isSortingSpecificColumns())
		{
			if (input.length() < _fromColumn)
			{
				// prevent an std::out_of_range exception
				return QString("");
			}
			else if (_fromColumn == _toColumn)
			{
				// get characters from the indicated column to the end of the line
				return input.mid(_fromColumn);
			}
			else
			{
				// get characters between the indicated columns, inclusive
				return input.mid(_fromColumn, _toColumn - _fromColumn);
			}
		}
		else
		{
			return input;
		}
	}

	bool isSortingSpecificColumns()
	{
		return _toColumn != 0;
	}

public:
	ISorter(bool isDescending, size_t fromColumn, size_t toColumn) : _isDescending(isDescending), _fromColumn(fromColumn), _toColumn(toColumn)
	{
		assert(_fromColumn <= _toColumn);
	};
	virtual ~ISorter() { };
	virtual QList<QString> sort(QList<QString> lines) = 0;
};

// Implementation of lexicographic sorting of lines.
class LexicographicSorter : public ISorter
{
public:
	LexicographicSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn) { };

	QList<QString> sort(QList<QString> lines) override
	{
		// Note that both branches here are equivalent in the sense that they always give the same answer.
		// However, if we are *not* sorting specific columns, then we get a 40% speed improvement by not calling
		// getSortKey() so many times.
		if (isSortingSpecificColumns())
		{
			std::sort(lines.begin(), lines.end(), [this](QString a, QString b)
			{
				if (isDescending())
				{
					return getSortKey(a).compare(getSortKey(b)) > 0;

				}
				else
				{
					return getSortKey(a).compare(getSortKey(b)) < 0;
				}
			});
		}
		else
		{
			std::sort(lines.begin(), lines.end(), [this](QString a, QString b)
			{
				if (isDescending())
				{
					return a.compare(b) > 0;
				}
				else
				{
					return a.compare(b) < 0;
				}
			});
		}
		return lines;
	}
};


// Implementation of lexicographic sorting of lines, ignoring character casing
class LexicographicCaseInsensitiveSorter : public ISorter
{
public:
	LexicographicCaseInsensitiveSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn) { };

	QList<QString> sort(QList<QString> lines) override
	{
		// Note that both branches here are equivalent in the sense that they always give the same answer.
		// However, if we are *not* sorting specific columns, then we get a 40% speed improvement by not calling
		// getSortKey() so many times.
		if (isSortingSpecificColumns())
		{
			std::sort(lines.begin(), lines.end(), [this](QString a, QString b)
			{
				if (isDescending())
				{
					return getSortKey(a).compare(getSortKey(b), Qt::CaseInsensitive) > 0;
				}
				else
				{
					return getSortKey(a).compare(getSortKey(b), Qt::CaseInsensitive) < 0;
				}
			});
		}
		else
		{
			std::sort(lines.begin(), lines.end(), [this](QString a, QString b)
			{
				if (isDescending())
				{
					return QString::compare(a, b, Qt::CaseInsensitive) > 0;
				}
				else
				{
					return QString::compare(a, b, Qt::CaseInsensitive) < 0;
				}
			});
		}
		return lines;
	}
};


class ReverseSorter : public ISorter
{
public:
	ReverseSorter(bool isDescending, size_t fromColumn, size_t toColumn) : ISorter(isDescending, fromColumn, toColumn) { };

	QList<QString> sort(QList<QString> lines) override
	{
		std::reverse(lines.begin(), lines.end());
		return lines;
	}
};