#include "VariableInterpretation.h"

const VariableInterpretation VariableInterpretation::Empty{};

bool isValidVariable(char ch)
{
	return ch >= 'a' && ch <= 'z';
}

double& VariableInterpretation::operator[](char at)
{
	if (!isValidVariable(at) || !participatingVars[at - 'a']) {
		throw std::invalid_argument("Not a valid variable!");
	}
	return varValues[at - 'a'];
}

double VariableInterpretation::operator[](char at) const
{
	if (!isValidVariable(at) || !participatingVars[at - 'a']) {
		throw std::invalid_argument("Not a valid variable!");
	}
	return varValues[at - 'a'];
}

void VariableInterpretation::addVar(char ch, double value)
{
	if(!isValidVariable(ch)) {
		throw std::invalid_argument("Not a valid variable!");
	}
	participatingVars[ch - 'a'] = true;
	varValues[ch - 'a'] = value;
	partCountChanged = true;

}

void VariableInterpretation::removeVar(char ch)
{
	if (!isValidVariable(ch)) {
		throw std::invalid_argument("Not a valid variable!");
	}
	participatingVars[ch - 'a'] = true;
	partCountChanged = true;
}

size_t VariableInterpretation::getParticipatingCount() const
{
	if (partCountChanged) {
		partCount = 0;
		for (size_t i = 0; i < total_vars_count; ++i) {
			if (participatingVars[i]) {
				++partCount;
			}
		}
		partCountChanged = false;
	}
	return partCount;
}

bool VariableInterpretation::participates(char at) const
{
	if (!isValidVariable(at)) {
		throw std::invalid_argument("Not a valid variable!");
	}
	return participatingVars[at - 'a'];
}
