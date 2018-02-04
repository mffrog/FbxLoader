//
//  FbxLoaderDataStructs.h
//  FbxLoader
//
//  Created by Tomoya Fujii on 2018/02/02.
//  Copyright © 2018年 TomoyaFujii. All rights reserved.
//

#ifndef FbxLoaderDataStructs_h
#define FbxLoaderDataStructs_h

#include "Vector.h"
#include "Matrix.h"
#include <vector>

namespace myTools{
    namespace FbxLoader {
        struct StaticVertex{
            Vector3 position;
            Vector4 color = {1,1,1,1};
            Vector2 texCoord;
            Vector3 normal;
            Vector4 tangent;
        };
        
        struct SkinnedVertex {
            Vector3 position;
            Vector4 color = Vector4(1,1,1,1);
            Vector2 texCoord;
            Vector3 normal;
            Vector4 tangent;
            unsigned int boneIndex[4] = {0,0,0,0};
            Vector4 weights = {0.0,0.0,0.0,0.0};
        };
        
        //コントロールポイント毎のウェイト取得
        struct PerCpBoneIndexAndWeight{
            std::vector<std::pair<int, double>> weights;
        };
        
        //各Boneのデータ
        struct BoneData{
            std::string name;
            int boneId = -1;
            int parentId = -1;
            std::vector<int> children;
            Matrix4x4 baseInv;
        };
        
        struct BoneTreeData{
            std::vector<BoneData> data;
            BoneData* FindBone(std::string name) {
                for(auto itr = data.begin(); itr != data.end(); ++itr){
                    if(itr->name == name){
                        return &(*itr);
                    }
                }
                return nullptr;
            }
            
            BoneData* FindBone(int boneId) {
                for(auto itr = data.begin(); itr != data.end(); ++itr){
                    if(itr->boneId == boneId){
                        return &(*itr);
                    }
                }
                return nullptr;
            }
        };
        
        template<typename VertType>
        struct Material{
            std::string name;
            std::vector<unsigned int> indeces;
            std::vector<VertType> verteces;
            std::vector<std::string> textureName;
        };
        
        struct StaticMesh{
            std::string name;
            std::vector<Material<StaticVertex>> materials;
        };
        
        struct SkinnedMesh{
            std::string name;
            std::vector<Material<SkinnedVertex>> materials;
        };
        
        struct Relation{
            /**
             * インデックスバッファの値
             */
            std::vector<int> relatedIndex;
        };
        
        struct BoneAnimationData{
            typedef std::pair<float,Matrix4x4> TimeMatPair;
            std::vector<std::pair<float, Matrix4x4>> animDatas;
            int current = -1;
            int next = -1;
            float animationTime = 0;
            Matrix4x4 GetMat(float time){
                if(!animDatas.size()){
                    return {};
                }
                
                if(animDatas.size() == 1){
                    return animDatas[0].second;
                }
                
                if(time > animationTime){
                    while(time > animationTime){
                        time -= animationTime;
                    }
                    current = 0;
                    next = 1;
                }
                
                if(current == -1){
                    for(int i = 0; i < animDatas.size(); ++i){
                        if(time < animDatas[i].first){
                            next = i;
                            current = i - 1;
                            break;
                        }
                    }
                }
                
                if(time < animDatas[current].first){
                    Reset();
                }
                
                if(current == -1){
                    for(int i = 0; i < animDatas.size(); ++i){
                        if(time < animDatas[i].first){
                            next = i;
                            current = i - 1;
                            break;
                        }
                    }
                }
                
                while(time > animDatas[next].first){
                    current++;
                    next++;
                    if(next >= animDatas.size()){
                        next = animDatas.size() - 1;
                        current = next - 1;
                        return animDatas.back().second;
                    }
                }
                
                float duration = animDatas[next].first - animDatas[current].first;
                float rate = (time - animDatas[current].first) / duration;
                if(rate > 1 || rate < 0){
                    std::cout << "irregular happened" << std::endl;
                }
                return animDatas[current].second * (1 - rate) + animDatas[next].second * rate;
            }
            
            void Reset(){
                current = -1;
                next = -1;
            }
        };
        
        struct Animation{
            float animationTime = 0;
            //[bone]
            std::vector<BoneAnimationData> boneAnimationData;
            
            std::vector<Matrix4x4> GetMat(float time){
                int boneNum = boneAnimationData.size();
                std::vector<Matrix4x4> mat(boneNum);
                for(int i = 0; i < boneNum; ++i){
                    if(time > animationTime || time == 0.0f){
                        boneAnimationData[i].Reset();
                        time = 0;
                    }
                    mat[i] = boneAnimationData[i].GetMat(time);
                }
                return mat;
            }
        };
        
    }// namespace FbxLoader
}// namespace myTools

#endif /* FbxLoaderDataStructs_h */

