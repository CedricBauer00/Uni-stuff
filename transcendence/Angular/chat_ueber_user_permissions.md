# User Permissions

## Aktueller Stand

- Gruppenmitglieder werden im Info-Modal angezeigt.
- Permissions der Gruppe werden geladen über:
  `GET /user-permission/:groupId/permissions`
- Permissions werden pro `userId` gefiltert.
- Das Edit-Permissions-Modal zeigt vier Checkboxen:
  - `documents.read`
  - `documents.upload`
  - `documents.update`
  - `documents.delete`
- Bereits vorhandene Permissions sind beim Öffnen angehakt.
- Checkboxänderungen werden lokal in `selectedPermissions` gespeichert.
- `save()` berechnet `permissionsToAdd` und `permissionsToRemove`.

## Noch offen

- POST-Methode zum Hinzufügen:
  `POST /user-permission/:groupId/permissions`
- DELETE-Methode zum Entfernen:
  `DELETE /user-permission/:groupId/permissions/:userId/:permKey`
- `save()` muss die Requests ausführen.
- Nach erfolgreichem Speichern Modal schließen und die Permissions-Liste neu laden.