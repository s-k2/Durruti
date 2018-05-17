#include "TreeDiff.h"

namespace TreeDiff {

template <class T>
class Comparator
{
public:
	Comparator(const std::string &leftPath, const std::string &rightPath, T &sink)
		: leftDiff(leftPath), rightDiff(rightPath), sink(sink)
	{
		processDirs();
	}

private:
	Reader leftDiff;
	Reader rightDiff;
	T &sink;

	void processDirs()
	{
		do {
			readTilEqual();
			if(isFileType(rightDiff.getCurrentType()))
				sink.conflict(leftDiff, rightDiff);//.getCurrentDir() + rightDiff.getCurrentFile());
		} while(leftDiff.getCurrentType() != Type::EndOfFile);
	}

	void readTilEqual()
	{
		rightDiff.readNext();
		leftDiff.readNext();

		int compareResult;
		while((compareResult = compareEntry()) != 0) {
			if(compareResult > 0) {
				if(isFileType(rightDiff.getCurrentType()))
					sink.copyToLeft(rightDiff);//.getCurrentDir() + rightDiff.getCurrentFile());
				rightDiff.readNext();
			} else {
				if(isFileType(leftDiff.getCurrentType()))
					sink.copyToRight(leftDiff);//.getCurrentDir() + leftDiff.getCurrentFile());
				leftDiff.readNext();
			}
		}
	}

	int compareEntry()
	{
		int typeCompareResult = compareType();
		if(typeCompareResult != 0)
			return(typeCompareResult);
		
		else if(isFileType(leftDiff.getCurrentType()))
			return(leftDiff.getCurrentFile().compare(rightDiff.getCurrentFile()));

		else if(leftDiff.getCurrentType() == Type::ChangeDir)
			return(leftDiff.getCurrentDir().compare(rightDiff.getCurrentDir()));

		return(0);
	}

	int compareType()
	{
		if(isFileType(leftDiff.getCurrentType()) && isFileType(rightDiff.getCurrentType()))
			return(0);

		return((char) leftDiff.getCurrentType() - (char) rightDiff.getCurrentType());
	}
};

}
