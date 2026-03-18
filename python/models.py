from pydantic import BaseModel
from typing import List, Optional, Any

class MatrixRequest(BaseModel):
    type: str
    n: int
    matrixA: List[float]
    matrixB: Optional[List[float]] = None
    scalar: Optional[float] = None

    def check_size(self):
        expected = self.n * self.n
        if len(self.matrixA) != expected:
            return False, f"matrixA должна иметь {expected} элементов"
        if self.matrixB and len(self.matrixB) != expected:
            return False, f"matrixB должна иметь {expected} элементов"
        return True, "OK"


class MatrixResponse(BaseModel):
    success: bool
    operation: Optional[str] = None
    type: Optional[str] = None
    size: Optional[int] = None
    result: Optional[List[Any]] = None
    error: Optional[str] = None