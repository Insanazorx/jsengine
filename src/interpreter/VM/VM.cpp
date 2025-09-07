#pragma once
#include "VM.h"

#include "../../frontend/debug.h"
#include "../Bytecode/BytecodeGenerator.h"
#include "Register.h"

namespace js{
    namespace Interpreter {

            void VM::initialize () {
                m_exec_state->acc.set64(0);
                m_exec_state->pc.set64(0);
                m_exec_state->lr.set64(0);
                m_registers[0] = &m_exec_state->acc;
                m_registers[0xFF] = &m_exec_state->lr;
            }

            void VM::run() {
                initialize();

                auto command_length = 0;
                std::vector<uint8_t> current_command_args;
                Opcode opcode=NOP;
                while (opcode != HALT) {
                    opcode = static_cast<Opcode>(m_stream->at(m_exec_state->pc.get_address()));

                    command_length = get_bytecode_length(opcode);
                    if (command_length == -1) {
                        VERIFY_NOT_REACHED();
                    }
                    while (command_length--){
                        current_command_args.push_back(m_stream->at(m_exec_state->pc.get_address()));
                        m_exec_state->pc.advance();
                    }

                    switch (opcode) {
                        case LOAD_ACCUMULATOR_REG:
                            INVOKE_HANDLER(LOAD_ACCUMULATOR_REG, current_command_args[1]);
                            break;
                        case LOAD_ACCUMULATOR_IMM64:
                            //TODO: check if it can take 8 bytes properly for imms (is it proper way?)
                            INVOKE_HANDLER(LOAD_ACCUMULATOR_IMM64, *reinterpret_cast<uint64_t*>(current_command_args.data()+ 1));
                            break;
                        case LOAD_ACCUMULATOR_IMM32:
                            INVOKE_HANDLER(LOAD_ACCUMULATOR_IMM32, *reinterpret_cast<uint32_t*>(current_command_args.data() + 1));
                            break;
                        case LOAD_ACCUMULATOR_IMM16:
                            INVOKE_HANDLER(LOAD_ACCUMULATOR_IMM16, *reinterpret_cast<uint16_t*>(current_command_args.data() + 1));
                            break;
                        case LOAD_ACCUMULATOR_IMM8:
                            INVOKE_HANDLER(LOAD_ACCUMULATOR_IMM8, current_command_args[1]);
                            break;
                        case STORE_ACCUMULATOR:
                            INVOKE_HANDLER(STORE_ACCUMULATOR, current_command_args[1]);
                            break;
                        case ADD:
                            INVOKE_HANDLER(ADD, current_command_args[1]);
                            break;
                        case DEBUG_PRINT:
                            INVOKE_HANDLER(DEBUG_PRINT, current_command_args[1]);
                            break;
                        case HALT:
                            INVOKE_HANDLER(HALT);
                            break;
                        default :
                            std::cerr << "Unknown opcode: " << static_cast<int>(opcode) << std::endl;
                            VERIFY_NOT_REACHED();
                    }
                    current_command_args.clear();
                }

            }

            DECLARE_HANDLER(VM::LOAD_ACCUMULATOR_REG, Reg reg) {
                DEBUG("Entered LOAD_ACCUMULATOR_REG_HANDLER with reg id: " << static_cast<int>(reg));

                make_register_exist(reg);
                m_exec_state->acc.set64(m_registers[reg]->val64());

                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(VM::LOAD_ACCUMULATOR_IMM64, Imm64){}
            DECLARE_HANDLER(VM::LOAD_ACCUMULATOR_IMM32, Imm32 imm) {}
            DECLARE_HANDLER(VM::LOAD_ACCUMULATOR_IMM16, Imm16 imm) {}
            DECLARE_HANDLER(VM::LOAD_ACCUMULATOR_IMM8, Imm8 imm) {

                DEBUG("Entered LOAD_ACCUMULATOR_IMM8_HANDLER with value: " << static_cast<int>(imm));

                m_exec_state->acc.set8(imm);

                DEBUG("Accumulator now has value: " << static_cast<int>(m_exec_state->acc.val8()));
            }
            DECLARE_HANDLER(VM::STORE_ACCUMULATOR, Reg reg) {

                DEBUG("Entered STORE_ACCUMULATOR_HANDLER with value: " << static_cast<int>(reg));

                make_register_exist(reg);
                m_registers[reg]->set64(m_exec_state->acc.val64());

                DEBUG("Register " << static_cast<int>(reg) << " now has value: " << m_registers[reg]->val64());
            }

            DECLARE_HANDLER(VM::ADD, Reg reg) {
                DEBUG("Entered ADD_HANDLER with value: " << static_cast<int>(reg));

                make_register_exist(reg);
                m_exec_state->acc.add_to_value(m_registers[reg]);

                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(VM::SUBTRACT, Reg reg){}
            DECLARE_HANDLER(VM::MULTIPLY, Reg reg){}
            DECLARE_HANDLER(VM::DIVIDE, Reg reg){}
            DECLARE_HANDLER(VM::JUMP, Reg reg){}
            DECLARE_HANDLER(VM::COMPARE, Reg reg) {}
            DECLARE_HANDLER(VM::TEST, Reg reg){}
            DECLARE_HANDLER(VM::JUMP_IF_EQUAL, Reg reg){}
            DECLARE_HANDLER(VM::JUMP_IF_ZERO, Reg reg){}
            DECLARE_HANDLER(VM::JUMP_UNLESS_ZERO, Reg reg){}
            DECLARE_HANDLER(VM::CALL_FUNCTION, Name_Index name){}
            DECLARE_HANDLER(VM::LOAD_NAMED_PROPERTY, Object_Descriptor od, Name_Index name){}
            DECLARE_HANDLER(VM::LOAD_KEYED_PROPERTY, Object_Descriptor od, Reg reg){}
            DECLARE_HANDLER(VM::LOAD_INDEXED_PROPERTY, Object_Descriptor od, Slot slot){}
            DECLARE_HANDLER(VM::CREATE_OBJECT, Name_Index name){}
            DECLARE_HANDLER(VM::CREATE_CONTEXT){}
            DECLARE_HANDLER(VM::NOP){}
            DECLARE_HANDLER(VM::RETURN){}
            DECLARE_HANDLER(VM::XOR, Reg reg){}
            DECLARE_HANDLER(VM::AND, Reg reg){}
            DECLARE_HANDLER(VM::OR, Reg reg){}
            DECLARE_HANDLER(VM::NEGATE){}
            DECLARE_HANDLER(VM::MODULO, Reg reg){}
            DECLARE_HANDLER(VM::SHIFT_LEFT, Reg reg){}
            DECLARE_HANDLER(VM::SHIFT_RIGHT, Reg reg){};
            DECLARE_HANDLER(VM::CREATE_FUNCTION, Name_Index name){};
            DECLARE_HANDLER(VM::CREATE_ARRAY, Name_Index name){};
            DECLARE_HANDLER(VM::SAVE_STATE){};
            DECLARE_HANDLER(VM::RESTORE_STATE){};
            DECLARE_HANDLER(VM::DEBUG_BREAK){};
            DECLARE_HANDLER(VM::DEBUG_PRINT, Reg reg) {
                DEBUG("ENTERED DEBUG_PRINT_HANDLER: " << static_cast<int>(reg) );
                make_register_exist(reg);
                std::cout << "DEBUG_PRINT: Register " << static_cast<int>(reg) << " has value: " << m_registers[reg]->val64() << std::endl;
            };
            DECLARE_HANDLER(VM::HALT) {
                return;
            }
    }
}
