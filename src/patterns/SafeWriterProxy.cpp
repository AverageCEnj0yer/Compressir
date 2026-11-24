#include "patterns/SafeWriterProxy.h"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace {

std::filesystem::path canonicalLexical(const std::filesystem::path& path) {
    auto normalized = std::filesystem::absolute(path).lexically_normal();
    while (!normalized.empty() && normalized != normalized.root_path() &&
           (normalized.filename().empty() || normalized.filename() == ".")) {
        auto parent = normalized.parent_path();
        if (parent == normalized) {
            break;
        }
        normalized = parent;
    }
    return normalized;
}

} // namespace

SafeWriterProxy::SafeWriterProxy(std::unique_ptr<IWriter> realWriter,
                                 std::filesystem::path allowedBaseDir)
    : m_realWriter{std::move(realWriter)},
      m_allowedBaseDir{std::move(allowedBaseDir)} {
    if (!m_realWriter) {
        throw std::invalid_argument("Real writer cannot be null");
    }
    if (m_allowedBaseDir.empty()) {
        throw std::invalid_argument("Allowed base directory cannot be empty");
    }
}

bool SafeWriterProxy::isPathAllowed(const std::filesystem::path& path) const {
    if (path.empty() || path.filename().empty() ||
        path.filename() == "." || path.filename() == "..") {
        return false;
    }

    const auto base = canonicalLexical(m_allowedBaseDir);
    const auto target = canonicalLexical(path);
    const auto rel = target.lexically_relative(base);

    if (rel.empty() || rel == ".") {
        return false;
    }

    for (const auto& part : rel) {
        if (part == "..") {
            return false;
        }
    }
    return true;
}

bool SafeWriterProxy::writeData(const std::filesystem::path& path,
                                const std::vector<std::byte>& data) {
    if (!isPathAllowed(path)) {
        std::cerr << "[SafeWriterProxy] Blocked write outside allowed directory '"
                  << m_allowedBaseDir.string() << "': " << path.string() << '\n';
        return false;
    }

    return m_realWriter->writeData(path, data);
}
