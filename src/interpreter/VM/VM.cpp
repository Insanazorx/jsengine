#pragma once
#include "VM.h"

#include "../../frontend/debug.h"
#include "../Bytecode/BytecodeGenerator.h"
#include "Register.h"
#include "../RuntimeObjects/JSObject.h"

namespace js{
    namespace Interpreter {

            void VM::initialize () {
                m_exec_state->acc.set64(0);
                m_exec_state->pc.set64(0);
                m_exec_state->lr.set64(0);
                m_registers[0] = &m_exec_state->acc;
                m_registers[0xFF] = &m_exec_state->lr;
                if (m_exec_state->current_context->current_environment() == nullptr) {
                    auto global_env = LexicalEnvironment::Create();
                    m_exec_state->current_context->allocate_environment(global_env);
                }
            }

            void VM::run() {
                initialize();

                auto command_length = 0;
                std::vector<uint8_t> current_command_args;
                Opcode opcode=NOP;
                while (opcode != HALT) {
                    opcode = static_cast<Opcode>(m_program->at(m_exec_state->pc.get_address()));

                    command_length = get_bytecode_length(opcode);
                    if (command_length == -1) {
                        VERIFY_NOT_REACHED();
                    }
                    while (command_length--){
                        current_command_args.push_back(m_program->at(m_exec_state->pc.get_address()));
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
                        case SUBTRACT:
                            INVOKE_HANDLER(SUBTRACT, current_command_args[1]);
                            break;
                        case MULTIPLY:
                            INVOKE_HANDLER(MULTIPLY, current_command_args[1]);
                            break;
                        case DIVIDE:
                            INVOKE_HANDLER(DIVIDE, current_command_args[1]);
                            break;
                        case JUMP:
                            INVOKE_HANDLER(JUMP, current_command_args[1]);
                            break;
                        case COMPARE:
                            INVOKE_HANDLER(COMPARE, current_command_args[1]);
                            break;
                        case TEST:
                            INVOKE_HANDLER(TEST, current_command_args[1]);
                            break;
                        case JUMP_IF_EQUAL:
                            INVOKE_HANDLER(JUMP_IF_EQUAL, current_command_args[1]);
                            break;
                        case JUMP_IF_ZERO:
                            INVOKE_HANDLER(JUMP_IF_ZERO, current_command_args[1]);
                            break;
                        case JUMP_IFNOT_ZERO:
                            INVOKE_HANDLER(JUMP_IFNOT_ZERO, current_command_args[1]);
                            break;
                        case CALL_FUNCTION:
                            INVOKE_HANDLER(CALL_FUNCTION, *reinterpret_cast<uint16_t*>(current_command_args.data() + 1));
                            break;
                        case LOAD_NAMED_PROPERTY:
                            INVOKE_HANDLER(LOAD_NAMED_PROPERTY,
                                *reinterpret_cast<uint16_t*>(current_command_args.data() + 1));
                            break;
                        case LOAD_KEYED_PROPERTY:
                            INVOKE_HANDLER(LOAD_KEYED_PROPERTY,
                                *reinterpret_cast<uint16_t*>(current_command_args.data() + 1));
                            break;
                        case LOAD_INDEXED_PROPERTY:
                            INVOKE_HANDLER(LOAD_INDEXED_PROPERTY,
                                *reinterpret_cast<uint16_t*>(current_command_args.data() + 1));
                        case CREATE_OBJECT:
                            INVOKE_HANDLER(CREATE_OBJECT);
                            break;
                        case CREATE_CONTEXT:
                            INVOKE_HANDLER(CREATE_CONTEXT);
                            break;
                        case NOP:
                            INVOKE_HANDLER(NOP);
                            break;
                        case RETURN:
                            INVOKE_HANDLER(RETURN);
                            break;
                        case XOR:
                            INVOKE_HANDLER(XOR, current_command_args[1]);
                            break;
                        case AND:
                            INVOKE_HANDLER(AND, current_command_args[1]);
                            break;
                        case OR:
                            INVOKE_HANDLER(OR, current_command_args[1]);
                            break;
                        case NEGATE:
                            INVOKE_HANDLER(NEGATE);
                            break;
                        case MODULO:
                            INVOKE_HANDLER(MODULO, current_command_args[1]);
                            break;
                        case SHIFT_LEFT:
                            INVOKE_HANDLER(SHIFT_LEFT, current_command_args[1]);
                            break;
                        case SHIFT_RIGHT:
                            INVOKE_HANDLER(SHIFT_RIGHT, current_command_args[1]);
                            break;
                        case CREATE_FUNCTION:
                            INVOKE_HANDLER(CREATE_FUNCTION);
                            break;
                        case CREATE_ARRAY:
                            INVOKE_HANDLER(CREATE_ARRAY);
                            break;
                        case SAVE_STATE:
                            INVOKE_HANDLER(SAVE_STATE);
                            break;
                        case RESTORE_STATE:
                            INVOKE_HANDLER(RESTORE_STATE);
                            break;
                        case DEBUG_BREAK:
                            INVOKE_HANDLER(DEBUG_BREAK);
                            break;
                        case DEBUG_PRINT:
                            INVOKE_HANDLER(DEBUG_PRINT, current_command_args[1]);
                            break;
                        case JUMP_IFNOT_EQUAL:
                            INVOKE_HANDLER(JUMP_IFNOT_EQUAL, current_command_args[1]);
                            break;
                        case JUMP_IF_GT:
                            INVOKE_HANDLER(JUMP_IF_GT, current_command_args[1]);
                            break;
                        case JUMP_IF_LT:
                            INVOKE_HANDLER(JUMP_IF_LT, current_command_args[1]);
                            break;
                        case JUMP_IF_GTE:
                            INVOKE_HANDLER(JUMP_IF_GTE, current_command_args[1]);
                            break;
                        case JUMP_IF_LTE:
                            INVOKE_HANDLER(JUMP_IF_LTE, current_command_args[1]);
                            break;
                        case CLEAR_FLAGS:
                            INVOKE_HANDLER(CLEAR_FLAGS);
                            break;
                        case SET_ACC_IS_OD_FLAG:
                            INVOKE_HANDLER(SET_ACC_IS_OD_FLAG);
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

            DECLARE_HANDLER(LOAD_ACCUMULATOR_REG, Reg reg) {
                DEBUG("Entered LOAD_ACCUMULATOR_REG_HANDLER with reg id: " << static_cast<int>(reg));

                make_register_exist(reg);
                m_exec_state->acc.set64(m_registers[reg]->val64());

                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(LOAD_ACCUMULATOR_IMM64, Imm64 imm) {
                DEBUG("Entered LOAD_ACCUMULATOR_IMM64_HANDLER with value: " << imm);
                m_exec_state->acc.set64(imm);

                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(LOAD_ACCUMULATOR_IMM32, Imm32 imm) {
                DEBUG ("Entered LOAD_ACCUMULATOR_IMM32_HANDLER with value: " << imm);

                m_exec_state->acc.set32(imm);

                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(LOAD_ACCUMULATOR_IMM16, Imm16 imm) {
                DEBUG("Entered LOAD_ACCUMULATOR_IMM16_HANDLER with value: " << imm);

                m_exec_state->acc.set16(imm);

                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(LOAD_ACCUMULATOR_IMM8, Imm8 imm) {

                DEBUG("Entered LOAD_ACCUMULATOR_IMM8_HANDLER with value: " << imm);

                m_exec_state->acc.set8(imm);

                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(STORE_ACCUMULATOR, Reg reg) {

                DEBUG("Entered STORE_ACCUMULATOR_HANDLER with value: " << reg);

                make_register_exist(reg);
                m_registers[reg]->set64(m_exec_state->acc.val64());

                DEBUG("Register " << static_cast<int>(reg) << " now has value: " << m_registers[reg]->val64());
            }

            DECLARE_HANDLER(ADD, Reg reg) {
                DEBUG("Entered ADD_HANDLER with value: " << static_cast<int>(reg));

                make_register_exist(reg);
                m_exec_state->acc.add_to_value(m_registers[reg]);

                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(SUBTRACT, Reg reg) {
                DEBUG ("Entered SUBTRACT_HANDLER with value: " << static_cast<int>(reg));
                make_register_exist(reg);
                m_exec_state->acc.subtract_from_value( m_registers[reg] );
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(MULTIPLY, Reg reg) {
                DEBUG("Entered MULTIPLY_HANDLER with value: " << static_cast<int>(reg));
                make_register_exist(reg);
                m_exec_state->acc.multiply_with_value( m_registers[reg] );
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(DIVIDE, Reg reg) {
                DEBUG("Entered DIVIDE_HANDLER with value: " << static_cast<int>(reg));
                make_register_exist(reg);
                m_exec_state->acc.divide_with_value( m_registers[reg] );
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(JUMP, Reg reg) {
                DEBUG("Entered JUMP_HANDLER with value: " << static_cast<int>(reg));
                DEBUG("Program Counter had value: " << m_exec_state->pc.get_address());
                make_register_exist(reg);
                m_exec_state->pc.set64(m_registers[reg]->val64());
                DEBUG("Program Counter now has value: " << m_exec_state->pc.get_address());
            }
            DECLARE_HANDLER(COMPARE, Reg reg) {
                DEBUG ("ENTERED COMPARE_HANDLER with reg id: " << static_cast<int>(reg) );
                make_register_exist(reg);
                const auto acc_val = m_exec_state->acc.val64();
                const auto reg_val = m_registers[reg]->val64();
                m_exec_state->flags.flags_union.flags.FLAG_EQUAL = acc_val == reg_val;
                m_exec_state->flags.flags_union.flags.FLAG_ZERO = acc_val == 0;
                m_exec_state->flags.flags_union.flags.FLAG_GT = acc_val > reg_val;
                m_exec_state->flags.flags_union.flags.FLAG_LT = acc_val < reg_val;

                DEBUG("reg1 value: " << acc_val << ", reg2 value: " << reg_val
                    << ", FLAG_EQUAL: " << m_exec_state->flags.flags_union.flags.FLAG_EQUAL);
            }
            DECLARE_HANDLER(TEST, Reg reg) {
                DEBUG("ENTERED TEST_HANDLER with reg id: " << static_cast<int>(reg) );
                make_register_exist(reg);
                const auto acc_val = m_exec_state->acc.val64();
                const auto reg_val = m_registers[reg]->val64();
                m_exec_state->flags.flags_union.flags.FLAG_ZERO = acc_val - reg_val == 0;
                DEBUG("reg1 value: " << acc_val << ", reg2 value: " << reg_val
                    << ", FLAG_ZERO: " << m_exec_state->flags.flags_union.flags.FLAG_ZERO);
            }
            DECLARE_HANDLER(JUMP_IF_EQUAL, Reg reg) {
                DEBUG("ENTERED JUMP_IF_EQUAL_HANDLER with reg id: " << static_cast<int>(reg) );
                if (m_exec_state->flags.flags_union.flags.FLAG_EQUAL) {
                    DEBUG("FLAG_EQUAL is set, jumping to reg id: " << static_cast<int>(reg) );
                    make_register_exist(reg);
                    m_exec_state->pc.set64(m_registers[reg]->val64());
                    DEBUG("Program Counter now has value: " << m_exec_state->pc.get_address());
                } else {
                    DEBUG("FLAG_EQUAL is not set, not jumping.");
                }
            }
            DECLARE_HANDLER(JUMP_IF_ZERO, Reg reg) {
                DEBUG("ENTERED JUMP_IF_ZERO_HANDLER with reg id: " << static_cast<int>(reg) );
                if (m_exec_state->flags.flags_union.flags.FLAG_ZERO) {
                    DEBUG("FLAG_ZERO is set, jumping to reg id: " << static_cast<int>(reg) );
                    make_register_exist(reg);
                    m_exec_state->pc.set64(m_registers[reg]->val64());
                    DEBUG("Program Counter now has value: " << m_exec_state->pc.get_address());
                } else {
                    DEBUG("FLAG_ZERO is not set, not jumping.");
                }
            }
            DECLARE_HANDLER(JUMP_IFNOT_ZERO, Reg reg) {
                DEBUG("ENTERED JUMP_UNLESS_ZERO_HANDLER with reg id: " << static_cast<int>(reg) );
                if (!m_exec_state->flags.flags_union.flags.FLAG_ZERO) {
                    DEBUG("FLAG_ZERO is not set, jumping to reg id: " << static_cast<int>(reg) );
                    make_register_exist(reg);
                    m_exec_state->pc.set64(m_registers[reg]->val64());
                    DEBUG("Program Counter now has value: " << m_exec_state->pc.get_address());
                } else {
                    DEBUG("FLAG_ZERO is set, not jumping.");
                }
            }
            DECLARE_HANDLER(CALL_FUNCTION, Name_Index name){}
            DECLARE_HANDLER(LOAD_NAMED_PROPERTY, Name_Index name_idx) {
                DEBUG("ENTERED LOAD_NAMED_PROPERTY_HANDLER with name_index: " << name_idx );

                LexicalEnvironment* env = m_exec_state->current_context->current_environment();
                VERIFY(env,"Current environment is null in LOAD_NAMED_PROPERTY_HANDLER");

                Name_Index od = m_exec_state->acc.val16();
                VERIFY(od != 0,"Object descriptor not found in LOAD_NAMED_PROPERTY_HANDLER");

                JSObject* obj = m_exec_state->current_context->current_environment()->get_object_by_descriptor(od);
                VERIFY(obj,"Object not found in LOAD_NAMED_PROPERTY_HANDLER");

                std::shared_ptr<Value> val = obj->GetPropertyByName(name_idx);
                VERIFY(!val->isUndefined(),"Property not found in LOAD_NAMED_PROPERTY_HANDLER");

                m_exec_state->acc.LoadValue(*val);

                m_exec_state->flags.flags_union.flags.FLAG_IS_ACC_OD = false; // Accumulator now holds a direct value
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(LOAD_KEYED_PROPERTY, Reg reg) {
                DEBUG("ENTERED LOAD_KEYED_PROPERTY_HANDLER with reg id: " << static_cast<int>(reg) );
                make_register_exist(reg);

                auto od = m_exec_state->acc.val16();
                VERIFY(od != 0,"Object descriptor not found in LOAD_KEYED_PROPERTY_HANDLER");

                JSObject* obj = m_exec_state->current_context->current_environment()->get_object_by_descriptor(od);
                VERIFY(obj,"Object not found in LOAD_KEYED_PROPERTY_HANDLER");

                int idx = m_registers[reg]->val16();
                DEBUG("Index to load element from: " << idx);

                std::shared_ptr<Value> val = obj->GetElementByIndex(idx);
                VERIFY(!val->isUndefined(),"Element not found in LOAD_KEYED_PROPERTY_HANDLER");

                m_exec_state->acc.LoadValue(*val);
                m_exec_state->flags.flags_union.flags.FLAG_IS_ACC_OD = false; // Accumulator now holds a direct value
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(LOAD_INDEXED_PROPERTY, Slot slot){}
            DECLARE_HANDLER(CREATE_OBJECT) {
                DEBUG("ENTERED CREATE_OBJECT_HANDLER" );

                JSObject* js_obj = JSObject::Create();
                VERIFY(js_obj,"Object creation failed in CREATE_OBJECT_HANDLER");

                LexicalEnvironment* env=m_exec_state->current_context->current_environment();
                VERIFY(env,"Current environment is null in CREATE_OBJECT_HANDLER");

                Name_Index od  = env->create_object(js_obj, "Object_"+std::to_string(env->js_objects_count()));
                DEBUG("Object created and stored in environment with name_index: " << od);

                m_exec_state->acc.set16(od);
                m_exec_state->flags.flags_union.flags.FLAG_IS_ACC_OD = true;
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());

            }
            DECLARE_HANDLER(CREATE_CONTEXT){}
            DECLARE_HANDLER(NOP){}
            DECLARE_HANDLER(RETURN){}
            DECLARE_HANDLER(XOR, Reg reg) {
                DEBUG("ENTERED XOR_HANDLER with reg id: " << static_cast<int>(reg) );
                make_register_exist(reg);
                m_exec_state->acc.xor_with_value(m_registers[reg]);
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(AND, Reg reg) {
                DEBUG("ENTERED AND_HANDLER with reg id: " << static_cast<int>(reg) );
                make_register_exist(reg);
                m_exec_state->acc.and_with_value(m_registers[reg]);
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(OR, Reg reg) {
                DEBUG("ENTERED OR_HANDLER with reg id: " << static_cast<int>(reg) );
                make_register_exist(reg);
                m_exec_state->acc.or_with_value(m_registers[reg]);
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(NEGATE) {
                DEBUG("ENTERED NEGATE_HANDLER" );
                m_exec_state->acc.negate_value();
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(MODULO, Reg reg) {
                DEBUG("ENTERED MODULO_HANDLER with reg id: " << static_cast<int>(reg) );
                make_register_exist(reg);
                m_exec_state->acc.modulo_with_value(m_registers[reg]);
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(SHIFT_LEFT, Reg reg) {
                DEBUG("ENTERED SHIFT_LEFT_HANDLER with reg id: " << static_cast<int>(reg) );
                make_register_exist(reg);
                m_exec_state->acc.shift_left_with_value(m_registers[reg]->val8());
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            }
            DECLARE_HANDLER(SHIFT_RIGHT, Reg reg) {
                DEBUG("ENTERED SHIFT_RIGHT_HANDLER with reg id: " << static_cast<int>(reg) );
                make_register_exist(reg);
                m_exec_state->acc.shift_right_with_value(m_registers[reg]->val8());
                DEBUG("Accumulator now has value: " << m_exec_state->acc.val64());
            };
            DECLARE_HANDLER(CREATE_FUNCTION){};
            DECLARE_HANDLER(CREATE_ARRAY){};
            DECLARE_HANDLER(SAVE_STATE){};
            DECLARE_HANDLER(RESTORE_STATE){};
            DECLARE_HANDLER(DEBUG_BREAK){};
            DECLARE_HANDLER(DEBUG_PRINT, Reg reg) {
                DEBUG("ENTERED DEBUG_PRINT_HANDLER: " << static_cast<int>(reg) );
                make_register_exist(reg);
                std::cout << "DEBUG_PRINT: Register " << static_cast<uint64_t>(reg) << " has value: " << m_registers[reg]->val64() << std::endl;
            };

            DECLARE_HANDLER(JUMP_IFNOT_EQUAL, Reg reg) {
                DEBUG("ENTERED DEBUG_PRINT_HANDLER: "<< static_cast<int>(reg) );
                if (!m_exec_state->flags.flags_union.flags.FLAG_EQUAL) {
                    DEBUG("FLAG_EQUAL is not set, jumping to reg id: " << static_cast<int>(reg) );
                    make_register_exist(reg);
                    m_exec_state->pc.set64(m_registers[reg]->val64());
                    DEBUG("Program Counter now has value: " << m_exec_state->pc.get_address());
                } else {
                    DEBUG("FLAG_EQUAL is set, not jumping.");
                }
            }
            DECLARE_HANDLER(JUMP_IF_GT, Reg reg) {
                DEBUG("ENTERED JUMP_IF_GT_HANDLER with reg id: " << static_cast<int>(reg) );
                if (m_exec_state->flags.flags_union.flags.FLAG_GT) {
                    DEBUG("FLAG_GT is set, jumping to reg id: " << static_cast<int>(reg) );
                    make_register_exist(reg);
                    m_exec_state->pc.set64(m_registers[reg]->val64());
                    DEBUG("Program Counter now has value: " << m_exec_state->pc.get_address());
                } else {
                    DEBUG("FLAG_GT is not set, not jumping.");
                }
            }
            DECLARE_HANDLER(JUMP_IF_LT, Reg reg) {
                DEBUG("ENTERED JUMP_IF_LT_HANDLER with reg id: " << static_cast<int>(reg) );
                if (m_exec_state->flags.flags_union.flags.FLAG_LT) {
                    DEBUG("FLAG_LT is set, jumping to reg id: " << static_cast<int>(reg) );
                    make_register_exist(reg);
                    m_exec_state->pc.set64(m_registers[reg]->val64());
                    DEBUG("Program Counter now has value: " << m_exec_state->pc.get_address());
                } else {
                    DEBUG("FLAG_LT is not set, not jumping.");
                }
            }
            DECLARE_HANDLER(JUMP_IF_GTE, Reg reg) {
                DEBUG("ENTERED JUMP_IF_GTE_HANDLER with reg id: " << static_cast<int>(reg) );
                if (m_exec_state->flags.flags_union.flags.FLAG_GT || m_exec_state->flags.flags_union.flags.FLAG_EQUAL) {
                    DEBUG("FLAG_GT or FLAG_EQUAL is set, jumping to reg id: " << static_cast<int>(reg) );
                    make_register_exist(reg);
                    m_exec_state->pc.set64(m_registers[reg]->val64());
                    DEBUG("Program Counter now has value: " << m_exec_state->pc.get_address());
                } else {
                    DEBUG("Neither FLAG_GT nor FLAG_EQUAL is set, not jumping.");
                }
            }
            DECLARE_HANDLER(JUMP_IF_LTE, Reg reg) {
                DEBUG("ENTERED JUMP_IF_LTE_HANDLER with reg id: " << static_cast<int>(reg) );
                if (m_exec_state->flags.flags_union.flags.FLAG_LT || m_exec_state->flags.flags_union.flags.FLAG_EQUAL) {
                    DEBUG("FLAG_LT or FLAG_EQUAL is set, jumping to reg id: " << static_cast<int>(reg) );
                    make_register_exist(reg);
                    m_exec_state->pc.set64(m_registers[reg]->val64());
                    DEBUG("Program Counter now has value: " << m_exec_state->pc.get_address());
                } else {
                    DEBUG("Neither FLAG_LT nor FLAG_EQUAL is set, not jumping.");
                }
            }
            DECLARE_HANDLER(CLEAR_FLAGS) {
                DEBUG("ENTERED CLEAR_FLAGS_HANDLER" );
                m_exec_state->flags.flags_union.all_flags = static_cast<uint64_t>(0);
                DEBUG("All flags cleared." );
            }

            DECLARE_HANDLER(SET_ACC_IS_OD_FLAG) {
                DEBUG("ENTERED SET_ACC_IS_OD_FLAG_HANDLER");
                m_exec_state->flags.flags_union.flags.FLAG_IS_ACC_OD = true;
            }

            DECLARE_HANDLER(HALT) {
                DEBUG("ENTERED HALT_HANDLER, halting execution." );
                VERIFY_NOT_REACHED();
            }
    }
}
