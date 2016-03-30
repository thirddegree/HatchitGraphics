/**
**    Hatchit Engine
**    Copyright(c) 2015 ThirdDegree
**
**    GNU Lesser General Public License
**    This file may be used under the terms of the GNU Lesser
**    General Public License version 3 as published by the Free
**    Software Foundation and appearing in the file LICENSE.LGPLv3 included
**    in the packaging of this file. Please review the following information
**    to ensure the GNU Lesser General Public License requirements
**    will be met: https://www.gnu.org/licenses/lgpl.html
**
**/

/**
 * \class IPipeline
 * \ingroup HatchitGraphics
 *
 * \brief An interface to a pipeline that should be extended and implemented by a graphics language
 *
 * A pipeline contains all sort of information about how the renderer should render subsequent
 * render passes. This includes things like topology, MSAA, shaders etc.
 */
    
#pragma once
    
#include <ht_platform.h>
#include <ht_shader.h>
#include <ht_debug.h>
#include <ht_shadervariable.h>
#include <ht_resourceobject.h>

#include <map>
    
namespace Hatchit {
    
   namespace Graphics {
   
        enum PolygonMode
        {
            SOLID,
            LINE
        };
   
        enum CullMode
        {
            NONE,
            FRONT,
            BACK
        };
   
        //Describes options for the render state
        //Some options are not available such as front face winding order
        struct RasterizerState
        {
            PolygonMode polygonMode;           //How we want to render objects
            CullMode    cullMode;              //How we want to cull faces
            bool        frontCounterClockwise; //Determines if a triangle is front- or back-facing.
            bool        depthClampEnable;      //True to use depth clamping, false to clip primitives
            bool        discardEnable;         //True to discard primitives before rasterization
            float       lineWidth;             //How wide to render when using Line polygon mode
        };
   
        enum SampleCount
        {
            SAMPLE_1_BIT,
            SAMPLE_2_BIT,
            SAMPLE_4_BIT,
            SAMPLE_8_BIT,
            SAMPLE_16_BIT,
            SAMPLE_32_BIT,
            SAMPLE_64_BIT
        };
   
        //Describes the multisampling state of the pipeline
        struct MultisampleState
        {
            SampleCount samples;	        //How many bits per sample
            float       minSamples;		    //Min samples per fragment
            bool        perSampleShading;   //Shades per sample if true, per fragment if false
        };
   
       class HT_API IPipeline : public Resource::ResourceObject
        {
       public:
            virtual ~IPipeline() {};

			virtual bool VInitFromFile(File* file) = 0;
           
            //If we wanted to allow users to control blending states
            //virtual void VSetColorBlendAttachments(ColorBlendState* colorBlendStates) = 0;
            
            /* Set the rasterization state for this pipeline
            * \param rasterState A struct containing rasterization options
            */
            virtual void VSetRasterState(const RasterizerState& rasterState) = 0;
           
            /* Set the multisampling state for this pipeline
            * \param multiState A struct containing multisampling options
            */
            virtual void VSetMultisampleState(const MultisampleState& multiState) = 0;
           
            /* Load a shader into a shader slot for the pipeline
            * \param shaderSlot The slot that you want the shader in; vertex, fragment etc.
            * \param shader A pointer to the shader that you want to load to the given shader slot
            */
            virtual void VLoadShader(ShaderSlot shaderSlot, IShader* shader) = 0;
           
            virtual bool VSetInt(std::string name, int data) = 0;
            virtual bool VSetFloat(std::string name, float data) = 0;
            virtual bool VSetFloat3(std::string name, Math::Vector3 data) = 0;
            virtual bool VSetFloat4(std::string name, Math::Vector4 data) = 0;
            virtual bool VSetMatrix4(std::string name, Math::Matrix4 data) = 0;

            ///Prepare the pipeline after you've set up your settings
            virtual bool VPrepare() = 0;

            ///Update the pipeline after you've changed the uniform data
            virtual bool VUpdate() = 0;

        protected:
            std::map<std::string, ShaderVariable*> m_shaderVariables;
        };
    }
}