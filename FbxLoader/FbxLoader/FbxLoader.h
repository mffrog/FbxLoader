//
//  FbxLoader.h
//  FbxLoader
//
//  Created by Tomoya Fujii on 2018/01/30.
//  Copyright © 2018年 TomoyaFujii. All rights reserved.
//

#ifndef FbxLoader_h
#define FbxLoader_h

#include <fbxsdk.h>
#include "FbxLoaderDataStructs.h"
#include <string>
#include <vector>



namespace myTools{
    namespace FbxLoader {
        
        class FbxLoader {
        public:
            
            ~FbxLoader();
            bool Initialize(std::string filename);
            void LoadBone(BoneTreeData& boneTree);
            void LoadAllMesh(std::vector<StaticMesh>& staticMeshes, std::vector<SkinnedMesh>& skinnedMeshes);
            void LoadAnimation(std::vector<Animation>& animations);
            
        private:
            
            fbxsdk::FbxNode* FindRootBone(fbxsdk::FbxNode* node);
            fbxsdk::FbxMesh* FindIncludedMesh(fbxsdk::FbxCluster* cluster);
            void LoadSkinnedMesh(fbxsdk::FbxMesh* mesh, SkinnedMesh& meshRef);
            void LoadStaticeMesh(fbxsdk::FbxMesh* mesh, StaticMesh& meshRef);
            
            
            bool isBoneTreeInitialized = false;
            bool boneBaseGetFromLink = false;
            BoneTreeData publicBoneTree;
            
            fbxsdk::FbxManager* pManager = nullptr;
            fbxsdk::FbxImporter* pImporter = nullptr;
            fbxsdk::FbxScene* pScene = nullptr;
            
            std::vector<fbxsdk::FbxMesh*> includedMeshes;
        };
        
    }// namespace FbxLoader
}// namespace myTools
#endif /* FbxLoader_h */

