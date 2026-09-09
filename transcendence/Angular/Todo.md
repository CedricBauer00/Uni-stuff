Wir arbeiten am Transcendence-Projekt.
Aktueller Branch: feature/frontend/user-group-permissions/ced

Bisher umgesetzt:
- Gruppenmitglieder können im Info-Modal hinzugefügt und entfernt werden.
- Gruppenmitglieder und ihre Permissions werden angezeigt.
- Permissions-Modal existiert.
- Die vier documents-Permissions werden als Checkboxen angezeigt.
- Bereits vorhandene Permissions werden beim Öffnen angehakt.
- Checkboxänderungen werden lokal in selectedPermissions gespeichert.
- save() berechnet permissionsToAdd und permissionsToRemove.

Noch offen:
- POST- und DELETE-Methoden im Groups-Service.
- save() muss die Add-/Remove-Requests ausführen.
- Nach erfolgreichem Speichern Modal schließen und Permissions im Info-Modal neu laden.