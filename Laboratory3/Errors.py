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


class AssignmentErrorLeft(ProgramError):
    message = "Incorrect value to the left of the assignment!"


class AssignmentErrorRight(ProgramError):
    message = "Incorrect value to the left of the assignment!"


class ErrorIndexes(ProgramError):
    message = "Incorrect array indexes!"


class IndexOutOfRange(ProgramError):
    message = "Array index out of range!"



