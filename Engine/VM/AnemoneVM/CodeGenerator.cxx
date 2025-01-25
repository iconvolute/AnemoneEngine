#include "AnemoneVM/CodeGenerator.hxx"
#include "AnemoneRuntime/Diagnostics/Assert.hxx"

namespace Anemone::VM
{
    Label CodeGenerator::DefineLabel()
    {
        Label index{.Index = this->_label_definitions.size()};
        this->_label_definitions.emplace_back();
        return index;
    }

    // Marks label location in code
    void CodeGenerator::MarkLabel(Label label)
    {
        AE_ASSERT(label.Index < this->_label_definitions.size());
        AE_ASSERT(this->_code.size() <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));
        this->_label_definitions[label.Index].Offset = static_cast<int32_t>(this->_code.size());
    }

    // Finishes processing labels. Determines if all labels are reachable.
    bool CodeGenerator::FinishLabels()
    {
        return true;
    }

    void CodeGenerator::EmitByte(std::byte value)
    {
        this->_code.emplace_back(value);
    }

    void CodeGenerator::EmitValueUInt8(uint8_t value)
    {
        this->EmitByte(static_cast<std::byte>(value));
    }

    void CodeGenerator::EmitValueUInt16(uint16_t value)
    {
        this->EmitByte(static_cast<std::byte>(value));
        this->EmitByte(static_cast<std::byte>(value >> 8));
    }

    void CodeGenerator::EmitValueUInt32(uint32_t value)
    {
        this->EmitByte(static_cast<std::byte>(value));
        this->EmitByte(static_cast<std::byte>(value >> 8));
        this->EmitByte(static_cast<std::byte>(value >> 16));
        this->EmitByte(static_cast<std::byte>(value >> 24));
    }

    void CodeGenerator::EmitValueUInt64(uint64_t value)
    {
        this->EmitByte(static_cast<std::byte>(value));
        this->EmitByte(static_cast<std::byte>(value >> 8));
        this->EmitByte(static_cast<std::byte>(value >> 16));
        this->EmitByte(static_cast<std::byte>(value >> 24));
        this->EmitByte(static_cast<std::byte>(value >> 32));
        this->EmitByte(static_cast<std::byte>(value >> 40));
        this->EmitByte(static_cast<std::byte>(value >> 48));
        this->EmitByte(static_cast<std::byte>(value >> 56));
    }

    void CodeGenerator::EmitValueInt8(int8_t value)
    {
        this->EmitValueUInt8(static_cast<uint8_t>(value));
    }

    void CodeGenerator::EmitValueInt16(int16_t value)
    {
        this->EmitValueUInt16(static_cast<uint16_t>(value));
    }

    void CodeGenerator::EmitValueInt32(int32_t value)
    {
        this->EmitValueUInt32(static_cast<uint32_t>(value));
    }

    void CodeGenerator::EmitValueInt64(int64_t value)
    {
        this->EmitValueUInt64(static_cast<uint64_t>(value));
    }

    void CodeGenerator::EmitValueFloat(float value)
    {
        this->EmitValueUInt32(std::bit_cast<uint32_t>(value));
    }

    void CodeGenerator::EmitValueDouble(double value)
    {
        this->EmitValueUInt64(std::bit_cast<uint64_t>(value));
    }

    void CodeGenerator::EmitToken(int32_t value)
    {
        this->EmitValueInt32(value);
    }

    void CodeGenerator::EmitOpcode(uint32_t value)
    {
        if (value <= 0xFF)
        {
            this->EmitValueUInt8(static_cast<uint8_t>(value));
        }
        else if (value <= 0xFFFF)
        {
            this->EmitValueUInt16(static_cast<uint8_t>(value));
        }
        else
        {
            this->EmitValueUInt32(static_cast<uint8_t>(value));
        }
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode)
    {
        this->EmitOpcode(opcode.Value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, uint8_t value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueUInt8(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, uint16_t value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueUInt16(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, uint32_t value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueUInt32(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, uint64_t value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueUInt64(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, int8_t value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueInt8(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, int16_t value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueInt16(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, int32_t value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueInt32(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, int64_t value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueInt64(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, float value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueFloat(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, double value)
    {
        this->EmitOpcode(opcode.Value);
        this->EmitValueDouble(value);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, Label label)
    {
        AE_ASSERT(label.Index < this->_label_definitions.size());
        AE_ASSERT(this->_code.size() <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));

        this->EmitOpcode(opcode.Value);

        // Get label definition.
        LabelDefinition& definition = this->_label_definitions[label.Index];

        // Define reference to the current place in code
        LabelReference reference{
            .NextReference = definition.FirstReference,
            .Offset = static_cast<int32_t>(this->_code.size()),
        };

        definition.FirstReference = static_cast<int32_t>(this->_label_references.size());

        this->_label_references.emplace_back(reference);

        // Emit placeholder for label offset.
        this->EmitToken(-1);
    }

    void CodeGenerator::EmitOpcode(OpCodeDefinition const& opcode, std::span<Label> labels)
    {
        // Emit opcode.
        this->EmitOpcode(opcode);
        this->EmitValueUInt32(static_cast<uint32_t>(labels.size()));

        // Each label reference in array is resolved against it position in the code. VM must use IP of the opcode and index of the label to calculate offset.

        for (Label const& label : labels)
        {
            AE_ASSERT(label.Index < this->_label_definitions.size());
            AE_ASSERT(this->_code.size() <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));

            // Get label definition.
            LabelDefinition& definition = this->_label_definitions[label.Index];

            // Define reference to the current place in code
            LabelReference reference{
                .NextReference = definition.FirstReference,
                .Offset = static_cast<int32_t>(this->_code.size()),
            };

            definition.FirstReference = static_cast<int32_t>(this->_label_references.size());

            this->_label_references.emplace_back(reference);

            // Emit placeholder for label offset.
            this->EmitToken(-1);
        }
    }
}
