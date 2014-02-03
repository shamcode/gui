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

#include "defaultscene.h"
#include "eventmouseclick.h"
#include "core/objects/2d/basic2dentity.h"

#include <vector>

#include "core/ogl/ogl.h"

#include "core/manager/guimanager.h"
#include "core/factory/shaderfactory.h"
#include "core/objects/abstractentity.h"

namespace core {

DefaultScene::DefaultScene() : AbstractScene()
{
}

DefaultScene::~DefaultScene()
{
}

void DefaultScene::prepareGL()
{
    CGUIManager::getInstance()->readGui("scripts/test.lua");

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,  GL_NICEST);
}

bool DefaultScene::updateGL()
{
    return true;
}

bool DefaultScene::updateGL(CEventMouseClick *e)
{
    Basic2dEntity *object = CGUIManager::getInstance()->getRootObject();
    object->onClicked(*e);
    if (!Basic2dEntity::objects4Event.empty()) {
        bool clicked;
        do {
            clicked = CGUIManager::getInstance()->runOnClickFor(Basic2dEntity::objects4Event.back());
            Basic2dEntity::objects4Event.pop_back();
            if (Basic2dEntity::objects4Event.empty())
                break;
        } while (!clicked);
    }
    return true;
}

void DefaultScene::paintGL()
{
    // TODO: После рисования 3d сцены чистить не нужно!
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CShader *shader = CShaderFactory::getInstance()->getShader("test");
    if (shader) {
        shader->bind();
        const std::vector<std::shared_ptr<AbstractEntity> > &objects = CGUIManager::getInstance()->getObjects();
        for (int i = objects.size()-1; i >= 0; --i)
            if (objects[i]->isRoot())
                objects[i]->paint();
        shader->disable();
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

} // namespace Core
