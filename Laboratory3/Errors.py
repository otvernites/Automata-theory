class ProgramError(Exception):
    message = "An unknown exception occurred"

    def __init__(self):
        super(ProgramError, self).__init__()
        self._error_string = self.message

    def __str__(self):
        return self._error_string


class VariableAlreadyExists(ProgramError):
    message = "A variable with the same name already exists!"


class NonExistentVariable(ProgramError):
    message = "Referencing a non-existent variable!"


class UninitializedVariable(ProgramError):
    message = "Referencing an uninitialized variable!"


class NotIntVariable(ProgramError):
    message = "The index is not an integer!"


class AssignmentError(ProgramError):
    message = "Cannot assign a value of this type to a variable!"


class AssignmentErrorLeft(ProgramError):
    message = "Incorrect value to the left of the assignment!"


class AssignmentErrorRight(ProgramError):
    message = "Incorrect value to the right of the assignment!"


class ErrorIndexes(ProgramError):
    message = "Incorrect array indexes!"


class IndexOutOfRange(ProgramError):
    message = "Array index out of range!"


class IdentificationError(ProgramError):
    message = "Identification error!"


class ArithmeticError(ProgramError):
    message = "The variable is not an integer!"


class LogicError(ProgramError):
    message = "The Pierce arrow is defined for several variables! (>=2)"


class NotBooleanVariable(ProgramError):
    message = "The boolean expression must contain only T or F values!"




