Get-ChildItem -Path ".\Engine", ".\Samples", ".\Tests", ".\Tools" -Include "*.cxx", "*.hxx", "*.inl" -Recurse | ForEach-Object {
    Write-Host $_.FullName
    clang-format.exe -i -style=file:"./.clang-format" $_.FullName
}
