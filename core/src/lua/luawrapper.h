/*
 * Labs4Physics - visualisation of physics process
 * Copyright (C) 2013  Leyko Sergey powt81lsyu@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LUAWRAPPER_H
#define LUAWRAPPER_H

#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include <typeindex>
#include <algorithm>

#include <iostream>

#include "lua/lua.h"

/**
 * @brief вспомогательное пространство имен. Не используйте его.
 */
namespace __CLuaWrapper {
static std::map<std::type_index, std::string> types;

template<typename Arg>
Arg *checkUserData(lua_State *l, int numArg)  {
    return *static_cast<Arg **>(luaL_checkudata(l, numArg, __CLuaWrapper::types[std::type_index(typeid(Arg))].c_str()));
}

template<typename Arg>
Arg checkType(lua_State *l, int numArg)  {
    return **static_cast<Arg **>(luaL_checkudata(l, numArg, __CLuaWrapper::types[std::type_index(typeid(Arg))].c_str()));
}

template<>
inline int checkType <int> (lua_State *l, int numArg) {
    return static_cast<int>(luaL_checkinteger(l, numArg));
}

template<>
inline float checkType <float> (lua_State *l, int numArg) {
    return static_cast<float>(luaL_checknumber(l, numArg));
}

template<>
inline double checkType <double> (lua_State *l, int numArg) {
    return luaL_checknumber(l, numArg);
}

template<>
inline std::string checkType <std::string> (lua_State *l, int numArg) {
    return luaL_checkstring(l, numArg);
}

template<>
inline const char *checkType <const char*> (lua_State *l, int numArg) {
    return luaL_checkstring(l, numArg);
}

template<typename Arg>
void pushType(lua_State *l, Arg arg) {
    Arg **fromLua = static_cast<Arg **>(lua_newuserdata(l, sizeof(Arg*)));
    *fromLua = new Arg();
    (**fromLua) = arg;
    luaL_getmetatable(l, __CLuaWrapper::types[std::type_index(typeid(Arg))].c_str());
    lua_setmetatable(l, -2);

}

template<>
inline void pushType(lua_State *l, const std::string &arg) {
    lua_pushstring(l, arg.c_str());
}

template<>
inline void pushType(lua_State *l, const char *arg) {
    lua_pushstring(l, arg);
}

template<>
inline void pushType(lua_State *l, int arg) {
    lua_pushinteger(l, arg);
}

template<>
inline void pushType(lua_State *l, float arg) {
    lua_pushnumber(l, arg);
}

template<>
inline void pushType(lua_State *l, double arg) {
    lua_pushnumber(l, arg);
}

// Arg(T::*func)() const
template<typename T, typename Arg, typename Fun, int id>
class GetterAdapter {
public:
    explicit GetterAdapter(Fun fun) : fun(fun) {}

    int operator() (lua_State *l) {
        T *t = __CLuaWrapper::checkUserData<T>(l, 1);
        Arg param = (t->*fun)();
        __CLuaWrapper::pushType<Arg>(l, param);
        return 1;
    }
private:
    Fun fun;
}; // class GetterAdapter

// void(T::*func)(Arg)
template<typename T, typename Arg, typename Fun, int id>
class SetterAdapter {
public:
    explicit SetterAdapter(Fun fun) : fun(fun) {}

    int operator() (lua_State *l) {
        T *t = __CLuaWrapper::checkUserData<T>(l, 1);
        Arg arg = __CLuaWrapper::checkType<Arg>(l, 2);
        (t->*fun)(arg);
        return 1;
    }
private:
    Fun fun;
}; // class SetterAdapter

// Arg T::*m
template<typename T, typename Arg, typename Member, int id>
class GetMemberAdapter {
public:
    explicit GetMemberAdapter(Member m) : m(m) {}

    int operator() (lua_State *l) {
        T *t = __CLuaWrapper::checkUserData<T>(l, 1);
        Arg param = t->*m;
        __CLuaWrapper::pushType<Arg>(l, param);
        return 1;
    }
private:
    Member m;
}; // class GetMemberAdapter

// Arg T::*m
template<typename T, typename Arg, typename Member, int id>
class SetMemberAdapter {
public:
    explicit SetMemberAdapter(Member m) : m(m) {}

    int operator() (lua_State *l) {
        T *t = __CLuaWrapper::checkUserData<T>(l, 1);
        Arg arg = __CLuaWrapper::checkType<Arg>(l, 2);
        t->*m = arg;
        return 1;
    }
private:
    Member m;
}; // class SetMemberAdapter

template<typename T, typename Arg, typename Fun, int id>
lua_CFunction getterAdapterInC(Fun fun)
{
    static GetterAdapter<T, Arg, Fun, id> my_functor(fun);
    return [](lua_State *l) {
        return my_functor(l);
    };
} // getterAdapterInC

template<typename T, typename Arg, typename Fun, int id>
lua_CFunction setterAdapterInC(Fun fun)
{
    static SetterAdapter<T, Arg, Fun, id > my_functor(fun);
    return [](lua_State *l) {
        return my_functor(l);
    };
} // setterAdapterInC

template<typename T, typename Arg, typename Member, int id>
lua_CFunction getMemberAdapterInC(Member m)
{
    static GetMemberAdapter<T, Arg, Member, id> my_functor(m);
    return [](lua_State *l) {
        return my_functor(l);
    };
} // getterAdapterInC

template<typename T, typename Arg, typename Member, int id>
lua_CFunction setMemberAdapterInC(Member m)
{
    static SetMemberAdapter<T, Arg, Member, id> my_functor(m);
    return [](lua_State *l) {
        return my_functor(l);
    };
} // setterAdapterInC

} // namespace __CLuaWrapper


/**
 * @brief Обертка для загрузки объектов из с++ в lua.
 * @n
 * Пример использования:
 * @code
 * CLuaWrapper<CBorder> b(lua, "border");
 * b.addConstructor();
 * b.addProperty<float, 1>("width", &CBorder::width);
 * b.addProperty<glm::vec3, 2>("color", &CBorder::color);
 * b.addDestructor();
 * b.complete(true);
 * @endcode
 */
template <typename T>
class CLuaWrapper
{
public:
    CLuaWrapper(lua_State *lua, const std::string &name) : lua(lua), nameOfGlobal(name),
        nameOfMetaTable(std::string("luaL_") + name), nsp("ui"), cntArgConstr(), sync(false),
        addChild(false), lReg(), lRegMemHelper(), properties()  {
    }

    /**
     * @brief Сгенерировать конструктор класса
     */
    void addConstructor() {
        __CLuaWrapper::types[std::type_index(typeid(T))] = nameOfMetaTable;
        cntArgConstr = 0;
        luaL_Reg constructor = {
            "new", [](lua_State *l) {
                T **userData = newUserData<T>(l);
                *userData = new T;
                luaL_getmetatable(l, __CLuaWrapper::types[std::type_index(typeid(T))].c_str());
                lua_setmetatable(l, -2);
                return 1;
            }
        };
        lReg.push_back(constructor);
    }

    /**
     * @brief Сгенерировать конструктор класса
     */
    template <typename Arg>
    void addConstructor() {
        __CLuaWrapper::types[std::type_index(typeid(T))] = nameOfMetaTable;
        cntArgConstr = 1;
        luaL_Reg constructor = {
            "new", [](lua_State *l) {
                Arg arg = __CLuaWrapper::checkType<Arg>(l, 1);
                T **userData = newUserData<T>(l);
                *userData = new T(arg);
                luaL_getmetatable(l, __CLuaWrapper::types[std::type_index(typeid(T))].c_str());
                lua_setmetatable(l, -2);
                return 1;
            }
        };
        lReg.push_back(constructor);
    }

    /**
     * @brief Сгенерировать конструктор класса
     */
    template <typename Arg1, typename Arg2>
    void addConstructor() {
        __CLuaWrapper::types[std::type_index(typeid(T))] = nameOfMetaTable;
        cntArgConstr = 2;
        luaL_Reg constructor = {
            "new", [](lua_State *l) {
                Arg1 arg1 = __CLuaWrapper::checkType<Arg1>(l, 1);
                Arg2 arg2 = __CLuaWrapper::checkType<Arg2>(l, 2);
                T **userData = newUserData<T>(l);
                *userData = new T(arg1, arg2);
                luaL_getmetatable(l, __CLuaWrapper::types[std::type_index(typeid(T))].c_str());
                lua_setmetatable(l, -2);
                return 1;
            }
        };
        lReg.push_back(constructor);
    }

    /**
     * @brief Сгенерировать конструктор класса
     */
    template <typename Arg1, typename Arg2, typename Arg3>
    void addConstructor() {
        __CLuaWrapper::types[std::type_index(typeid(T))] = nameOfMetaTable;
        cntArgConstr = 3;
        luaL_Reg constructor = {
            "new", [](lua_State *l) {
                Arg1 arg1 = __CLuaWrapper::checkType<Arg1>(l, 1);
                Arg2 arg2 = __CLuaWrapper::checkType<Arg2>(l, 2);
                Arg2 arg3 = __CLuaWrapper::checkType<Arg2>(l, 3);
                T **userData = newUserData<T>(l);
                *userData = new T(arg1, arg2, arg3);
                luaL_getmetatable(l, __CLuaWrapper::types[std::type_index(typeid(T))].c_str());
                lua_setmetatable(l, -2);
                return 1;
            }
        };
        lReg.push_back(constructor);
    }

    /**
     * @brief Добавить свойство типа Arg с именем name
     */
    template <typename Arg, int id>
    void addProperty(const std::string &name, Arg(T::*getter)() const, void(T::*setter)(const Arg&)) {
        properties.push_back(std::pair<std::string, bool>(name,
                __CLuaWrapper::types.find(std::type_index(typeid(Arg))) == __CLuaWrapper::types.end()));
        lRegMemHelper.push_back(std::string("get") + name);
        luaL_Reg get = {
            lRegMemHelper.back().c_str(),
             __CLuaWrapper::getterAdapterInC<T, Arg, Arg(T::*)() const, id >(getter)
        };
        lRegMemHelper.push_back(std::string("set") + name);
        luaL_Reg set = {
           lRegMemHelper.back().c_str(),
            __CLuaWrapper::setterAdapterInC<T, Arg, void(T::*)(const Arg&), id > (setter)
        };
        lReg.push_back(get);
        lReg.push_back(set);
    }

    /**
     * @brief Добавить свойство типа Arg с именем name
     */
    template <typename Arg, int id>
    void addProperty(const std::string &name, Arg(T::*getter)() const, void(T::*setter)(Arg)) {
        properties.push_back(std::pair<std::string, bool>(name,
                __CLuaWrapper::types.find(std::type_index(typeid(Arg))) == __CLuaWrapper::types.end()));
        lRegMemHelper.push_back(std::string("get") + name);
        luaL_Reg get = {
            lRegMemHelper.back().c_str(),
             __CLuaWrapper::getterAdapterInC<T, Arg, Arg(T::*)() const, id>(getter)
        };
        lRegMemHelper.push_back(std::string("set") + name);
        luaL_Reg set = {
           lRegMemHelper.back().c_str(),
            __CLuaWrapper::setterAdapterInC<T, Arg, void(T::*)(Arg), id > (setter)
        };
        lReg.push_back(get);
        lReg.push_back(set);
    }

    /**
     * @brief Добавить свойство типа Arg с именем name
     */
    template <typename Arg, int id>
    void addProperty(const std::string &name, Arg(T::*getter)(), void(T::*setter)(Arg)) {
        properties.push_back(std::pair<std::string, bool>(name,
                __CLuaWrapper::types.find(std::type_index(typeid(Arg))) == __CLuaWrapper::types.end()));
        lRegMemHelper.push_back(std::string("get") + name);
        luaL_Reg get = {
            lRegMemHelper.back().c_str(),
             __CLuaWrapper::getterAdapterInC<T, Arg, Arg(T::*)(), id >(getter)
        };
        lRegMemHelper.push_back(std::string("set") + name);
        luaL_Reg set = {
           lRegMemHelper.back().c_str(),
            __CLuaWrapper::setterAdapterInC<T, Arg, void(T::*)(Arg), id > (setter)
        };
        lReg.push_back(get);
        lReg.push_back(set);
    }

    /**
     * @brief Добавить свойство типа Arg с именем name
     */
    template <typename Arg, int id>
    void addProperty(const std::string &name, Arg T::*member) {
        properties.push_back(std::pair<std::string, bool>(name,
                __CLuaWrapper::types.find(std::type_index(typeid(Arg))) == __CLuaWrapper::types.end()));
        lRegMemHelper.push_back(std::string("get") + name);
        luaL_Reg get = {
            lRegMemHelper.back().c_str(),
             __CLuaWrapper::getMemberAdapterInC<T, Arg, Arg T::*, id >(member)
        };
        lRegMemHelper.push_back(std::string("set") + name);
        luaL_Reg set = {
           lRegMemHelper.back().c_str(),
            __CLuaWrapper::setMemberAdapterInC<T, Arg, Arg T::*, id > (member)
        };
        lReg.push_back(get);
        lReg.push_back(set);
    }

    /**
     * @brief Добавить свойство типа Arg с именем name
     */
    void addProperty(const luaL_Reg &member) {
        if (std::string(member.name) == "sync")
            sync = true;
        if (std::string(member.name) == "addChild")
            addChild = true;
        lReg.push_back(member);
    }

    /**
     * @brief Добавить деструктор
     */
    void addDestructor() {
        luaL_Reg constructor = {
            "__gc", [](lua_State *l) {
                T *userData = __CLuaWrapper::checkUserData<T>(l, 1);
                delete userData;
                return 0;
            }
        };
        lReg.push_back(constructor);
    }

    /**
     * @brief Завершить конструирование обертки, сгенерировав все необходимое для
     * общения С++ - Lua
     * @param generateDeclarativeStuff - генерировать ли код для декларативного описания.
     * Если параметр установлен в истину, сгенерируется код, благодаря которому можно будет писать
     * вот так:
     * @code
     * ui:border
     * {
     *   color = ui:vec3(0.2, 0.2, 0.2);
     *   width = 0.02;
     * }
     * @endcode
     */
    void complete(bool generateDeclarativeStuff = false) {
        generateString4Declarative(generateDeclarativeStuff);
        bool ret = !luaL_dostring(lua, doStr.c_str());
        if (!ret) {
            std::string log(lua_tostring(lua, -1));
            std::cout << log;
        }

        lReg.push_back({NULL, NULL});
        luaL_newmetatable(lua, nameOfMetaTable.c_str());
        luaL_setfuncs(lua, &lReg[0], 0);
        lua_pushvalue(lua, -1);
        lua_setfield(lua, -1, "__index");
        lua_setglobal(lua, nameOfGlobal.c_str());
    }

    /**
     * @brief Установить область имен для данного объекта
     */
    void setNameSpace(const std::string &nsp) {
        this->nsp = nsp;
    }

protected:
    template <typename Arg>
    static Arg** newUserData(lua_State *l) {
        return static_cast<Arg **>(lua_newuserdata(l, sizeof(Arg*)));
    }

private:
    CLuaWrapper(const CLuaWrapper &);
    const CLuaWrapper &operator= (const CLuaWrapper &);

    /**
     * @brief Генерирование дополнительного lua-кода для возможности декларативного описания интерфейса
     */
    void generateString4Declarative(bool generateDeclarativeStuff) {
        std::string argsConstr = "";
        for (int i = 0; i < cntArgConstr; ++i) {
            argsConstr += "arg" + std::to_string(i) + (i == cntArgConstr-1 ? "" : ", ");
        }

        doStr = "function " + nsp + ":" + nameOfGlobal + "(" + (argsConstr.empty() ? "data" : argsConstr) + ")\n";
        doStr += "  local r = " + nameOfGlobal + ".new(" +argsConstr + ")\n";

        if (generateDeclarativeStuff) {
            if (cntArgConstr)
                doStr += "  return function(data)\n";
            doStr += "  local ret = {}\n";
            doStr += "  for k, v in pairs(data) do\n";
            doStr += "    if k == \"" + properties[0].first + "\" then\n";
            doStr += "      r:set" + properties[0].first + (properties[0].second ? "(v)\n" : "(v.obj)\n");
            for (int i = 1, n = properties.size(); i < n; ++i) {
                doStr += "    elseif k == \"" + properties[i].first + "\" then\n";
                doStr += "      r:set" + properties[i].first + (properties[i].second ? "(v)\n" : "(v.obj)\n");
            }
            doStr += "    elseif k == \"onClick\" then\n      ret.onClick = v\n";
            doStr += "    elseif k == \"onPressed\" then\n      ret.onPressed = v\n";
            doStr += "    elseif k == \"onReleased\" then\n      ret.onReleased = v\n";

            if (addChild)
                doStr += "    else\n      r:addChild(v.obj)\n";
            doStr += "    end\n";
            doStr += "ret[k..\"_meta\"]=v\n";
            doStr +="  end\n";

            doStr += "  local mt = {}\n";
            doStr += "  mt.__index = function(self, key)\n";
            doStr += "    if key == \"" + properties[0].first + "\" then\n";
            doStr += "      return self[key..\"_meta\"]\n";//.obj:get" + properties[0].first + "()\n";
            for (int i = 1, n = properties.size(); i < n; ++i) {
                doStr += "    elseif key == \"" + properties[i].first + "\" then\n";
                doStr += "      return self[key..\"_meta\"]\n";//.obj:get" + properties[i].first + "()\n";
            }
            doStr += "    else\n      return rawget(self, key)\n    end\n  end\n";

            doStr += "  mt.__newindex = function(self, key, value)\n";
            doStr += "    if key == \"" + properties[0].first + "\" then\n";
            doStr += "      self.obj:set" + properties[0].first + (properties[0].second ? "(value)\n" : "(value.obj)\n");
            doStr += "      self[key..\"_meta\"] = value\n";
            for (int i = 1, n = properties.size(); i < n; ++i) {
                doStr += "    elseif key == \"" + properties[i].first + "\" then\n";
                doStr += "      self.obj:set" + properties[i].first + (properties[i].second ? "(value)\n" : "(value.obj)\n");
                doStr += "      self[key..\"_meta\"] = value\n";
            }
            doStr += "    end\n  end\n";

            if (sync)
                doStr += "  r:sync()\n";
            doStr += "  ret.obj = r\n";
            doStr += "  setmetatable(ret, mt)\n";

            bool containId = false;
            for(auto it: properties) {
                if (it.first == "id") {
                    containId = true;
                    break;
                }
            }
            if (containId)
                doStr += "  ui[arg0] = ret\n"; //TODO: придумать, как передать явно!

            doStr += "  return ret\n";
            if (cntArgConstr)
                doStr += "  end\nend\n";
            else
                doStr += "  end\n";
        } else {
            doStr += "  ret = {}\n";
            doStr += "  ret.obj = r\n";
            doStr += "  return ret\n";
            doStr += "  end\n";
        }


//        std::cout << doStr;
    }

    lua_State *lua;
    std::string nameOfGlobal;
    std::string nameOfMetaTable;
    std::string nsp;
    static std::string doStr;
    int cntArgConstr;
    bool sync;
    bool addChild;
    std::vector<luaL_Reg> lReg;
    std::vector<std::string> lRegMemHelper; // для корректного хранения строк в luaL_Reg
    std::vector< std::pair<std::string, bool> > properties;
};

template <typename T>
std::string CLuaWrapper<T>::doStr;

#endif // LUAWRAPPER_H
