GiD Post Results File 1.0

Result "Temperature" "Load Case 1" 1 Scalar OnNodes
ComponentNames "T"
Values
End Values

Result "Element_Fluxes" "Load Case 1" 1 Vector OnGaussPoints "GP_ELEMENT_1"
ComponentNames "Flux_x", "Flux_y", "Flux_z"
Values
End Values

Result "Average Flow" "Load Case 1" 1 Vector OnNodes
ComponentNames "Flux_x", "Flux_y", "Flux_z"
Values
End Values
