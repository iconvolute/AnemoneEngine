#pragma once

template <typename T>
struct AnemoneReflection_ToTypeId;

template <uint64_t Id>
struct AnemoneReflection_FromTypeId;

namespace Anemone::Reflection
{
    struct TypeId
    {
        uint64_t Inner;
        std::string_view Name;

        [[nodiscard]] static constexpr TypeId Create(std::string_view name)
        {
            return {
                .Inner = Anemone::Hash::FNV1A64::FromString(name),
                .Name = name,
            };
        }

        [[nodiscard]] constexpr bool operator==(TypeId const& other) const
        {
            return this->Inner == other.Inner;
        }

        [[nodiscard]] constexpr bool operator!=(TypeId const& other) const
        {
            return this->Inner != other.Inner;
        }

        [[nodiscard]] constexpr bool operator>(TypeId const& other) const
        {
            return this->Inner > other.Inner;
        }

        [[nodiscard]] constexpr bool operator<(TypeId const& other) const
        {
            return this->Inner < other.Inner;
        }

        [[nodiscard]] constexpr bool operator>=(TypeId const& other) const
        {
            return this->Inner >= other.Inner;
        }

        [[nodiscard]] constexpr bool operator<=(TypeId const& other) const
        {
            return this->Inner <= other.Inner;
        }
    };
}

namespace Anemone::Reflection
{
    struct MemberId
    {
        uint64_t Inner;
        std::string_view Name;

        static constexpr MemberId Create(std::string_view name)
        {
            return {
                .Inner = Anemone::Hash::FNV1A64::FromString(name),
                .Name = name,
            };
        }

        [[nodiscard]] constexpr bool operator==(MemberId const& other) const
        {
            return this->Inner == other.Inner;
        }

        [[nodiscard]] constexpr bool operator!=(MemberId const& other) const
        {
            return this->Inner != other.Inner;
        }

        [[nodiscard]] constexpr bool operator>(MemberId const& other) const
        {
            return this->Inner > other.Inner;
        }

        [[nodiscard]] constexpr bool operator<(MemberId const& other) const
        {
            return this->Inner < other.Inner;
        }

        [[nodiscard]] constexpr bool operator>=(MemberId const& other) const
        {
            return this->Inner >= other.Inner;
        }

        [[nodiscard]] constexpr bool operator<=(MemberId const& other) const
        {
            return this->Inner <= other.Inner;
        }
    };

    using PropertyGetter = ErrorCode(const void* instance, void* value);
    using PropertySetter = ErrorCode(void* instance, const void* value);

    struct PropertyDescriptor final
    {
        MemberId Name;
        TypeId Type;

        PropertyGetter* Getter;
        PropertySetter* Setter;

        template <typename T>
        ErrorCode InvokeGetter(void* instance, T& result)
        {
            if (this->Getter)
            {
                if (this->Type == AnemoneReflection_ToTypeId<T>::Id)
                {
                    return this->Getter(instance, &result);
                }

                return ErrorCode::OperationNotPermitted;
            }

            return ErrorCode::NotSupported;
        }

        template <typename T>
        ErrorCode InvokeSetter(void* instance, T const& value)
        {
            if (this->Setter)
            {
                if (this->Type == AnemoneReflection_ToTypeId<T>::Id)
                {
                    return this->Setter(instance, &value);
                }

                return ErrorCode::OperationNotPermitted;
            }

            return ErrorCode::NotSupported;
        }
    };

    template <typename T>
    struct TypedPropertyDescriptor final
    {
        // Only way to obtain these pointers is through valid calls to the reflection system.
    private:
        PropertyGetter* m_Getter;
        PropertySetter* m_Setter;

    public:
        explicit TypedPropertyDescriptor(PropertyGetter* getter, PropertySetter* setter)
            : m_Getter{getter}
            , m_Setter{setter}
        {
        }

        ErrorCode Get(void* instance, T& result)
        {
            return this->m_Getter(instance, &result);
        }

        ErrorCode Set(void* instance, T const& value)
        {
            return this->m_Setter(instance, &value);
        }
    };

    using EventHandlerMethod = ErrorCode(void* instance, void* value);

    struct EventHandlerDescriptor
    {
        MemberId Name;
        TypeId Type;

        EventHandlerMethod* Method;

        template <typename T>
        ErrorCode Invoke(void* instance, void* value)
        {
            if (this->Method)
            {
                if (this->Type == AnemoneReflection_ToTypeId<T>::Id)
                {
                    return this->Method(instance, value);
                }

                return ErrorCode::OperationNotPermitted;
            }

            return ErrorCode::NotSupported;
        }
    };

    class ClassBuilder final
    {
    private:
        TypeId _type_id;
        std::vector<PropertyDescriptor> _properties{};
        std::vector<EventHandlerDescriptor> _event_handlers{};

    public:
        explicit ClassBuilder(std::string_view name)
            : _type_id{TypeId::Create(name)}
        {
        }

    public:
        ClassBuilder& Property(std::string_view name, TypeId type, PropertyGetter* getter, PropertySetter* setter)
        {
            PropertyDescriptor descriptor{
                .Name = MemberId::Create(name),
                .Type = type,
                .Getter = getter,
                .Setter = setter,
            };

            this->_properties.emplace_back(descriptor);

            return *this;
        }

        ClassBuilder& EventHandler(std::string_view name, TypeId type, EventHandlerMethod* method)
        {
            EventHandlerDescriptor descriptor{
                .Name = MemberId::Create(name),
                .Type = type,
                .Method = method,
            };

            this->_event_handlers.emplace_back(descriptor);

            return *this;
        }

        PropertyDescriptor* GetProperty(MemberId name, TypeId type)
        {
            auto it = std::find_if(this->_properties.begin(), this->_properties.end(), [name, type](PropertyDescriptor const& descriptor)
            {
                return (descriptor.Name == name) and (descriptor.Type == type);
            });

            if (it != this->_properties.end())
            {
                return &(*it);
            }

            return nullptr;
        }

        PropertyDescriptor* GetProperty(MemberId name)
        {
            auto it = std::find_if(this->_properties.begin(), this->_properties.end(), [name](PropertyDescriptor const& descriptor)
            {
                return descriptor.Name == name;
            });

            if (it != this->_properties.end())
            {
                return &(*it);
            }

            return nullptr;
        }

        template <typename T>
        PropertyDescriptor* GetProperty(std::string_view name)
        {
            return this->GetProperty(MemberId::Create(name), AnemoneReflection_ToTypeId<T>::Id);
        }

        PropertyDescriptor* GetProperty(std::string_view name)
        {
            return this->GetProperty(MemberId::Create(name));
        }

        EventHandlerDescriptor* GetEventHandler(MemberId name, TypeId type)
        {
            auto it = std::find_if(this->_event_handlers.begin(), this->_event_handlers.end(), [name, type](EventHandlerDescriptor const& descriptor)
            {
                return (descriptor.Name == name) and (descriptor.Type == type);
            });

            if (it != this->_event_handlers.end())
            {
                return &(*it);
            }

            return nullptr;
        }

        EventHandlerDescriptor* GetEventHandler(MemberId name)
        {
            auto it = std::find_if(this->_event_handlers.begin(), this->_event_handlers.end(), [name](EventHandlerDescriptor const& descriptor)
            {
                return descriptor.Name == name;
            });

            if (it != this->_event_handlers.end())
            {
                return &(*it);
            }

            return nullptr;
        }

        EventHandlerDescriptor* GetEventHandler(std::string_view name)
        {
            return this->GetEventHandler(MemberId::Create(name));
        }

        template <typename T>
        EventHandlerDescriptor* GetEventHandler(std::string_view name)
        {
            return this->GetEventHandler(MemberId::Create(name), AnemoneReflection_ToTypeId<T>::Id);
        }
    };

    class SerializationContext;
    class EditorContext;

    class ReflectionContext
    {
    private:
        std::unique_ptr<ClassBuilder> _class{};

    public:
        virtual ~ReflectionContext() = default;

        virtual SerializationContext* ToSerializationContext()
        {
            return nullptr;
        }
        virtual EditorContext* ToEditorContext()
        {
            return nullptr;
        }

        ClassBuilder& Type(std::string_view name)
        {
            this->_class = std::make_unique<ClassBuilder>(name);
            return *this->_class;
        }

        ClassBuilder* Get()
        {
            return this->_class.get();
        }
    };

    class SerializationContext : public ReflectionContext
    {
    public:
        ~SerializationContext() override = default;

        SerializationContext* ToSerializationContext() override
        {
            return this;
        }
    };

    class EditorContext : public ReflectionContext
    {
    public:
        ~EditorContext() override = default;

        EditorContext* ToEditorContext() override
        {
            return this;
        }
    };
}

#define ANEMONE_REFLECTION_TYPE(T) \
    template <> \
    struct AnemoneReflection_ToTypeId<T> final \
    { \
        static constexpr auto Id = Anemone::Reflection::TypeId::Create(#T); \
    }; \
    template <> \
    struct AnemoneReflection_FromTypeId<Anemone::Reflection::TypeId::Create(#T).Inner> final \
    { \
        using Type = T; \
    };

ANEMONE_REFLECTION_TYPE(bool);
ANEMONE_REFLECTION_TYPE(int8_t);
ANEMONE_REFLECTION_TYPE(int16_t);
ANEMONE_REFLECTION_TYPE(int32_t);
ANEMONE_REFLECTION_TYPE(int64_t);
ANEMONE_REFLECTION_TYPE(uint8_t);
ANEMONE_REFLECTION_TYPE(uint16_t);
ANEMONE_REFLECTION_TYPE(uint32_t);
ANEMONE_REFLECTION_TYPE(uint64_t);
ANEMONE_REFLECTION_TYPE(float);
ANEMONE_REFLECTION_TYPE(double);
ANEMONE_REFLECTION_TYPE(std::string);
ANEMONE_REFLECTION_TYPE(char);


/*
namespace Example
{
    struct MyEventArgs
    {
        int32_t Value;
        bool Handled;
    };
}

ANEMONE_REFLECTION_TYPE(Example::MyEventArgs);

namespace Example
{
    class MyClass
    {
    private:
        int _field_int;
        float _field_float;

    public:
        std::string FieldString;

    public:
        int GetValue() const
        {
            return this->_field_int;
        }

        void SetValue(int value)
        {
            this->_field_int = value;
        }

        void OnEvent(MyEventArgs& args)
        {
            fmt::println("MyClass::OnEvent({})", args.Value);
        }

    public:
        static void Reflect(Anemone::Reflection::ReflectionContext& context)
        {
            context.Type("MyClass")
                .Property(
                    "Value",
                    AnemoneReflection_ToTypeId<decltype(_field_int)>::Id,
                    [](const void* instance, void* value) -> Anemone::ErrorCode
                    {
                        auto self = static_cast<const MyClass*>(instance);
                        *static_cast<int32_t*>(value) = self->GetValue();
                        return {};
                    },
                    [](void* instance, const void* value) -> Anemone::ErrorCode
                    {
                        auto self = static_cast<MyClass*>(instance);
                        self->SetValue(*static_cast<const int32_t*>(value));
                        return {};
                    })
                .Property(
                    "FieldString",
                    AnemoneReflection_ToTypeId<decltype(FieldString)>::Id,
                    [](const void* instance, void* value) -> Anemone::ErrorCode
                    {
                        auto self = static_cast<const MyClass*>(instance);
                        *static_cast<std::string*>(value) = self->FieldString;
                        return {};
                    },
                    [](void* instance, const void* value) -> Anemone::ErrorCode
                    {
                        auto self = static_cast<MyClass*>(instance);
                        self->FieldString = *static_cast<const std::string*>(value);
                        return {};
                    })
                .EventHandler(
                    "OnEvent",
                    AnemoneReflection_ToTypeId<MyEventArgs>::Id,
                    [](void* instance, void* value) -> Anemone::ErrorCode
                    {
                        auto self = static_cast<MyClass*>(instance);
                        auto args = static_cast<MyEventArgs*>(value);
                        self->OnEvent(*args);
                        return {};
                    });
        }
    };
}
*/
