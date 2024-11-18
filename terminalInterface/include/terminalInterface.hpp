#ifndef TERMINAL_INTERFACE_INCLUDE_TERMINAL_INTERFACE_HPP
#define TERMINAL_INTERFACE_INCLUDE_TERMINAL_INTERFACE_HPP

#include <stddef.h>

#include "terminalInterfaceErrors.hpp"
#include "../../akinatorLib/include/akinatorLib.hpp"

TerminalInterfaceErrors printDefinitionOfObject(const TypicalBinaryTree* tree, size_t pathLen, size_t* path);
TerminalInterfaceErrors isObjectGuessedCorrectly(const Node* node, bool* isCorrectGuess);
TerminalInterfaceErrors askQuestionFromNodesData(const Node* node, bool *isToLeftSon);
TerminalInterfaceErrors isAnswerOnQuestionYes(bool* isYes);
TerminalInterfaceErrors mainProgramWhileTrue(Akinator* akinator);

#endif
