#pragma once
#include "AnemoneRuntime.Interop/Headers.hxx"
#include "AnemoneRuntime.VM/OpCode.hxx"
#include "AnemoneRuntime.VM/Label.hxx"

#include <cstdint>
#include <vector>
#include <span>
#include <array>
#include <bit>
#include <cstddef>

namespace Anemone::VM
{
    class ANEMONE_RUNTIME_VM_API CodeGenerator
    {
    public:
        struct LabelReference
        {
            int32_t NextReference{-1};

            // Offset in code, where label is referenced
            int32_t Offset{-1};
        };

        struct LabelDefinition
        {
            int32_t FirstReference{-1};

            // Offset in code, where label is defined.
            int32_t Offset{-1};
        };

    private:
        std::vector<std::byte> _code{};
        std::vector<LabelDefinition> _label_definitions{};
        std::vector<LabelReference> _label_references{};

    public:
        // Defines label without code location
        Label DefineLabel();

        // Marks label location in code
        void MarkLabel(Label label);

        // Finishes processing labels. Determines if all labels are reachable.
        bool FinishLabels();

        void EmitByte(std::byte value);

        void EmitValueUInt8(uint8_t value);
        void EmitValueUInt16(uint16_t value);
        void EmitValueUInt32(uint32_t value);
        void EmitValueUInt64(uint64_t value);

        void EmitValueInt8(int8_t value);
        void EmitValueInt16(int16_t value);
        void EmitValueInt32(int32_t value);
        void EmitValueInt64(int64_t value);

        void EmitValueFloat(float value);
        void EmitValueDouble(double value);

        void EmitToken(int32_t value);

        void EmitOpcode(uint32_t value);

        void EmitOpcode(OpCodeDefinition const& opcode);
        void EmitOpcode(OpCodeDefinition const& opcode, uint8_t value);
        void EmitOpcode(OpCodeDefinition const& opcode, uint16_t value);
        void EmitOpcode(OpCodeDefinition const& opcode, uint32_t value);
        void EmitOpcode(OpCodeDefinition const& opcode, uint64_t value);
        void EmitOpcode(OpCodeDefinition const& opcode, int8_t value);
        void EmitOpcode(OpCodeDefinition const& opcode, int16_t value);
        void EmitOpcode(OpCodeDefinition const& opcode, int32_t value);
        void EmitOpcode(OpCodeDefinition const& opcode, int64_t value);
        void EmitOpcode(OpCodeDefinition const& opcode, float value);
        void EmitOpcode(OpCodeDefinition const& opcode, double value);

        void EmitOpcode(OpCodeDefinition const& opcode, Label label);
        void EmitOpcode(OpCodeDefinition const& opcode, std::span<Label> labels);
    };
}
