#pragma once

#include <QDir>
#include <QStringList>

namespace desktop {
QDir PortableRoot();
QString PortableExecutable(const QString &root);
int ReplacePortable(const QStringList &arguments);
void FinishPortableUpdate(const QStringList &arguments);
} // namespace desktop
