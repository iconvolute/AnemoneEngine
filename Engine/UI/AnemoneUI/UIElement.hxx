#pragma once
#include "AnemoneInterop/Headers.hxx"

namespace Anemone::UI
{
    class GeometryBuilder;
    enum class GeometryType : uint32_t;

    class UI_API UIElement
    {
    public:
        virtual ~UIElement() = default;

    protected:
        virtual bool OnUpdate(float deltaTime) = 0;
        virtual bool OnDraw(GeometryBuilder& builder, GeometryType type) = 0;
    };
}
