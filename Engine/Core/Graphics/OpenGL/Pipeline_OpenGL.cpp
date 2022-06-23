#include "Graphics\OpenGL\Pipeline_OpenGL.h"
#include "Graphics\OpenGL\Vertex_OpenGL.h"
#include "Graphics\OpenGL\ShaderProgram_OpenGL.h"

namespace gl
{
    namespace utils
    {
        GLenum FromTopologyType(TopologyMode mode)
        {
            switch (mode)
            {
                case TopologyMode::POINTS:
                    return GL_POINTS;
                case TopologyMode::LINES:
                    return GL_LINES;
                case TopologyMode::TRIANGLES:
                    return GL_TRIANGLES;
                default:
                    return GL_NONE;
            }
        }
    }

    Pipeline::Ref Pipeline::Create(
          std::shared_ptr<GraphicsAPI> graphics
        , PipelineSpecification const& spec)
    {
        return std::make_shared<Pipeline>(spec);
    }

    Pipeline::Pipeline(PipelineSpecification const& spec)
        : specification(spec)
    {

    }
    
    void Pipeline::Submit(
          ICommand::Ref command
        , std::shared_ptr<IVertexArray> stream
        , u64 vertexCount/* = 0*/)
    {
        // VERY IMPORTANT
        // THE PIPELINE NEEDS TO BIND _SOME_ SHADER
        auto shader = ShaderProgram::Cast(specification.shader);
        glUseProgram(shader->id);

        for (auto& textureObject : specification.textures)
        {
            shader->UploadTexture(textureObject.texture, textureObject.binding);
        }

        auto mesh = VertexArray::Cast(stream);

        // if our mesh has no vertices, then bail
        if (mesh->vertexCount == 0u)
            return;

        glBindVertexArray(mesh->id);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vertexBuffer->id);
        
        if (mesh->indexBuffer != nullptr)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->indexBuffer->id);
            glDrawElements(
                  utils::FromTopologyType(specification.topologyMode)
                , mesh->indexCount
                , GL_UNSIGNED_INT
                , nullptr);
        }
        else
        {
            glDrawArrays(
                  utils::FromTopologyType(specification.topologyMode)
                , 0
                , vertexCount == 0 ? mesh->vertexCount : vertexCount);
        }
    }

    std::shared_ptr<IVertexArray> Pipeline::CreateVertexArray(
          u64 vertexCount
        , void const* vertices)
    {
        return VertexArray::Create(specification.input, specification.stride(), vertexCount, vertices);
    }
}