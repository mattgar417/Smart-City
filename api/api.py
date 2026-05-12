import subprocess, json
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from fastapi.responses import FileResponse
from pydantic import BaseModel, Field, validator
from typing import Optional, List

app = FastAPI(title="Smart Resource Allocator API")

app.mount("/static", StaticFiles(directory="static"), name="static")

@app.get("/")
def root():
    return FileResponse("static/index.html")

class ResourceEntry(BaseModel):
    type: str = Field(..., description="Electricity | Water | Gas | Waste")
    district: str
    cost: float = Field(..., gt=0)
    importance: int = Field(..., ge=1, le=10)
    availability: float = Field(..., ge=0.0, le=1.0)
    additional1: Optional[float] = None
    additional2: Optional[float] = None

    @validator("type")
    def validate_type(cls, v):
        return v
    
class AllocationRequest(BaseModel):
    resources: List[ResourceEntry] = Field(..., min_items=1)

def resource_to_csv_line(r: ResourceEntry) -> str:
    line = f"{r.type},{r.district},{r.cost},{r.importance},{r.availability}"
    if r.additional1 is not None:
        line += f",{r.additional1}"
    if r.additional2 is not None:
        line += f",{r.additional2}"
    return line

@app.post("/allocate")
def allocate(request: AllocationRequest):
    stdin_payload = "\n".join(resource_to_csv_line(r) for r in request.resources)
    stdin_payload += "\n\n"

    result = subprocess.run(
        ["./city_allocator"],  # compiled C++ binary
        input=stdin_payload,
        capture_output=True, 
        text=True
    )

    output = json.loads(result.stdout)
    
    return {
        "status": "ok",
        "resources_loaded": output["resources_loaded"],
        "resources_skipped": output["resources_skipped"],
        "allocation": output["allocation"]  # the formatted allocation text from C++
    }

@app.get("/health")
def health():
    return {"status": "healthy"}
