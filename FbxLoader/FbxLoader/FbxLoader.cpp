//
//  FbxLoader.cpp
//  FbxLoader
//
//  Created by Tomoya Fujii on 2018/01/30.
//  Copyright © 2018年 TomoyaFujii. All rights reserved.
//

#include "FbxLoader.h"
#include <time.h>

using namespace fbxsdk;

namespace myTools{
    namespace FbxLoader {
        
        //Fbxから変換
        Matrix4x4 toMyMat(const FbxAMatrix& fbxmat){
            Matrix4x4 ret;
            for(int row = 0; row < 4; ++row){
                for(int col = 0; col < 4; ++col){
                    ret[row][col] = fbxmat.Get(row, col);
                }
            }
            return ret;
        }
        
        //アトリビュート取得
        template <typename T>
        T GetElement
        (
         FbxGeometryElement::EMappingMode mappingMode,
         bool isDirectRef,
         const FbxLayerElementArrayTemplate<int>* pIndexList,
         const FbxLayerElementArrayTemplate<T>* pList,
         int cpIndex,
         int polygonVertx,
         const T& defaultValue
         ){
            switch (mappingMode) {
                case fbxsdk::FbxLayerElement::eByControlPoint:
                    return (*pList)[isDirectRef ? cpIndex : (*pIndexList)[cpIndex]];
                case fbxsdk::FbxLayerElement::eByPolygonVertex:
                    return (*pList)[isDirectRef ? polygonVertx : (*pIndexList)[polygonVertx]];
                default:
                    return defaultValue;
            }
        }
        
        /**
         * デストラクタ
         */
        FbxLoader::~FbxLoader(){
            if(pImporter){
                pImporter->Destroy();
            }
            if(pManager){
                pManager->Destroy();
            }
        }
        
        /**
         * 初期化関数
         *
         * @param   filename    読み込むファイル名
         * @retval  true : 初期化成功 false : 初期化に失敗
         */
        bool FbxLoader::Initialize(std::string filename){
            pManager = FbxManager::Create();
            
            FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
            pManager->SetIOSettings(ios);
            
            pImporter = FbxImporter::Create(pManager, "");
            
            if(!pImporter->Initialize(filename.c_str(), -1, pManager->GetIOSettings())){
                printf("Call to FbxImporter::Initialize() failed.\n");
                printf("Error returned: %s\n\n", pImporter->GetStatus().GetErrorString());
                pImporter->Destroy();
                pManager->Destroy();
                return false;
            }
            
            pScene = FbxScene::Create(pManager, "");
            pImporter->Import(pScene);
            
            FbxGeometryConverter gConverter(pManager);
            gConverter.Triangulate(pScene, true);
            
            return true;
        }
        
        /**
         * ボーンのルートを探す
         *
         * @param node 検索対象(Sceneからルートを渡す)
         */
        FbxNode* FbxLoader::FindRootBone(FbxNode* node){
            int attrCount = node->GetNodeAttributeCount();
            for(int i = 0; i < attrCount; ++i){
                FbxNodeAttribute* attr = node->GetNodeAttributeByIndex(i);
                if(attr && attr->GetAttributeType() == FbxNodeAttribute::eSkeleton){
                    FbxSkeleton* skeleton = static_cast<FbxSkeleton*>(attr);
                    if(skeleton->IsSkeletonRoot()){
                        return node;
                    }
                }
            }
            
            int childCount = node->GetChildCount();
            for(int i = 0; i < childCount; ++i){
                FbxNode* ret = FindRootBone(node->GetChild(i));
                if(ret){
                    return ret;
                }
            }
            return nullptr;
        }
        
        /**
         * クラスターに影響を受けるメッシュの取得
         *
         * @param cluster   メッシュを取得したいクラスター
         */
        FbxMesh* FbxLoader::FindIncludedMesh(FbxCluster* cluster){
            int meshCount = pScene->GetSrcObjectCount<FbxMesh>();
            if(!includedMeshes.size()){
                for(int i = 0; i < meshCount; ++i){
                    includedMeshes.push_back(pScene->GetSrcObject<FbxMesh>(i));
                }
            }
            for (int i = 0; i < meshCount; ++i) {
                
                FbxMesh* mesh = includedMeshes[i];
                if(!mesh->GetDeformerCount(FbxDeformer::eSkin)){
                    continue;
                }
                
                FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin));
                int clusterCount = skin->GetClusterCount();
                for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex) {
                    FbxCluster* meshCluster = skin->GetCluster(clusterIndex);
                    if(meshCluster == cluster){
                        return mesh;
                    }
                }
            }
            return nullptr;
        }
        
        
        /**
         * ボーンのデータを読み込む
         *
         * @param boneTree  データの保存先
         * @tips    ボーンのデータはFbxLoaderStructsファイルを参照
         *
         * @tips    BoneのIndexを階層構造から決定する
         *          Weight設定時のインデックスはクラスターのリンクの名前でTreeDataからデータを検索してそこからindexを取得すること
         */

        void FbxLoader::LoadBone(BoneTreeData& boneTree){
            FbxNode* rootBone = FindRootBone(pScene->GetRootNode());
            if(!rootBone){
                return ;
            }
            struct GetSkeleton{
                static int Run(FbxNode* node, BoneTreeData& boneTree, int parentId){
                    if(node->GetNodeAttribute()->GetAttributeType() != FbxNodeAttribute::eSkeleton){
                        return -1;
                    }
                    BoneData boneData;
                    
                    boneData.boneId = boneTree.data.size();
                    boneData.parentId = parentId;
                    boneData.name = node->GetName();
                    
                    boneTree.data.push_back(boneData);
                    
                    int childCount = node->GetChildCount();
                    for(int i = 0; i < childCount; ++i){
                        boneData.children.push_back(Run(node->GetChild(i),boneTree,boneData.boneId));
                    }
                    return boneData.boneId;
                }
            };
            
            auto GetGeometry = [](FbxNode* pNode)->FbxAMatrix
            {
                const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
                const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
                const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
                
                return FbxAMatrix(lT, lR, lS);
            };
            
            GetSkeleton::Run(rootBone, boneTree, -1);
            int clusterCount = pScene->GetSrcObjectCount<FbxCluster>();
            for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex){
                FbxCluster* cluster = pScene->GetSrcObject<FbxCluster>(clusterIndex);
                const FbxMesh* includedMesh = FindIncludedMesh(cluster);
                if(includedMesh && !boneBaseGetFromLink){
                    FbxAMatrix a,b,c;
                    cluster->GetTransformLinkMatrix(a);
                    cluster->GetTransformMatrix(b);
                    c = GetGeometry(includedMesh->GetNode());
                    BoneData* pData = boneTree.FindBone(cluster->GetLink()->GetName());
                    if(pData){
                        pData->baseInv = toMyMat(a.Inverse() * b * c);
                    }
                }
                else {
                    BoneData* pData = boneTree.FindBone(cluster->GetLink()->GetName());
                    if(pData){
                        FbxAMatrix tmp;
                        cluster->GetTransformLinkMatrix(tmp);
                        pData->baseInv = toMyMat(tmp.Inverse());
                    }
                }
            }
            publicBoneTree = boneTree;
        }
        
        
        /**
         * ファイルに含まれているメッシュを読み込む
         *
         * @param   staticMeshes    アニメーションをしないメッシュの格納先
         * @param   skinnedMeshes   アニメーションをするメッシュの格納先
         */
        void FbxLoader::LoadAllMesh(std::vector<StaticMesh>& staticMeshes, std::vector<SkinnedMesh>& skinnedMeshes){
            int meshCount = pScene->GetSrcObjectCount<FbxMesh>();
            for(int meshIndex = 0; meshIndex < meshCount; ++meshIndex){
                FbxMesh* mesh = pScene->GetSrcObject<FbxMesh>(meshIndex);
                if(mesh->GetDeformerCount(FbxDeformer::eSkin) > 0){
                    int smIndex = skinnedMeshes.size();
                    skinnedMeshes.push_back({});
                    LoadSkinnedMesh(pScene->GetSrcObject<FbxMesh>(meshIndex), skinnedMeshes[smIndex]);
                }
                else {
                    int smIndex = staticMeshes.size();
                    staticMeshes.push_back({});
                    LoadStaticeMesh(mesh, staticMeshes[smIndex]);
                }
            }
        }
        
        
        /**
         * アニメーションをするメッシュの読み込み
         *
         * @param   mesh    ノードから取得したメッシュ
         * @param   meshRef 読み込んだデータの格納先
         */
        void FbxLoader::LoadSkinnedMesh(FbxMesh* mesh, SkinnedMesh& meshRef){
            auto toVector4 = [](auto v){
                return Vector4(v[0],v[1],v[2],v[3]);
            };
            
            auto toVector3 = [](auto v){
                return Vector3(v[0],v[1],v[2]);
            };
            
            auto toVector2 = [](FbxVector2 v){
                return Vector2(v[0],v[1]);
            };
            
            //頂点数
            int cpCount = mesh->GetControlPointsCount();
            //頂点配列
            FbxVector4* controlPoints = mesh->GetControlPoints();
            
            int skinCount = mesh->GetDeformerCount(FbxDeformer::eSkin);
            
            //      差分
            //----------------------------------------------------------------------
            
            std::vector<PerCpBoneIndexAndWeight> cpWeights(cpCount);
            if(skinCount){
                for(int i = 0; i < skinCount; ++i){
                    FbxSkin* skin = static_cast<FbxSkin*>(mesh->GetDeformer(i, FbxDeformer::eSkin));
                    if(skin){
                        int clusterCount = skin->GetClusterCount();
                        for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex){
                            FbxCluster* cluster = skin->GetCluster(clusterIndex);
                            //                    switch (cluster->GetLinkMode()) {
                            //                        case fbxsdk::FbxCluster::eAdditive:
                            //                            std::cout << "additive" << std::endl;
                            //                            break;
                            //                        case fbxsdk::FbxCluster::eNormalize:
                            //                            std::cout << "normalize" << std::endl;
                            //                            break;
                            //                        default:
                            //                            std::cout << "total1" << std::endl;
                            //                            break;
                            //                    }
                            
                            BoneData* pData = publicBoneTree.FindBone(cluster->GetLink()->GetName());
                            
                            //影響を与える頂点インデックス(ControlPointのIndex)とそのWeightの取得
                            int relatedCpCount = cluster->GetControlPointIndicesCount();
                            int* relatedCpIndex = cluster->GetControlPointIndices();
                            double* weights = cluster->GetControlPointWeights();
                            for(int r = 0; r < relatedCpCount; ++r){
                                std::vector<std::pair<int, double>>& pair = cpWeights[relatedCpIndex[r]].weights;
                                
                                cpWeights[relatedCpIndex[r]].weights.push_back({pData ? pData->boneId : clusterIndex, weights[r]});
                            }
                        }
                    }
                }
                
                //ウェイトを４つに制限とウェイトを正規化
                for(auto& w : cpWeights){
                    double sum = 0;
                    int count = 0;
                    if(w.weights.size() > 4){
                        std::sort(w.weights.begin(), w.weights.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b){ return a.second > b.second;});
                        w.weights.erase(w.weights.begin() + 4, w.weights.end());
                    }
                    
                    for(auto& weight : w.weights){
                        sum += weight.second;
                        count++;
                    }
                    for(auto& weight : w.weights){
                        weight.second /= sum;
                    }
                }
            }
            
            //----------------------------------------------------------------------

            
            FbxNode* meshNode = mesh->GetNode();
            
            FbxAMatrix mat = meshNode->EvaluateGlobalTransform();
            FbxAMatrix rot(FbxVector4(0, 0, 0), mat.GetR(), FbxVector4(1,1,1));
            
            int materialCount = mesh->GetNode()->GetMaterialCount();
            std::cout << "material num : " << materialCount << std::endl;
            materialCount = materialCount ? materialCount : 1;
            
            auto& materials = meshRef.materials;
            materials.resize(materialCount);
            
            //テクスチャ取得
            for(int i = 0; i < materialCount; ++i){
                FbxSurfaceMaterial* material = meshNode->GetMaterial(i);
                if(material){
                    materials[i].name = material->GetName();
                    FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
                    int layerNum = property.GetSrcObjectCount<FbxLayeredTexture>();
                    if(0 < layerNum){
                        for(int j = 0; j < layerNum; ++j){
                            FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(j);
                            int textureCount = layeredTexture->GetSrcObjectCount<FbxFileTexture>();
                            for(int textureIndex = 0; textureIndex < textureCount; ++textureIndex){
                                FbxFileTexture* texture = layeredTexture->GetSrcObject<FbxFileTexture>(textureIndex);
                                if(texture){
                                    std::string textureName = texture->GetRelativeFileName();
                                    std::cout << textureName << std::endl;
                                    materials[i].textureName.push_back(textureName);
                                }
                            }
                        }
                    }
                    else {
                        int fileTextureCount = property.GetSrcObjectCount<FbxFileTexture>();
                        if(0 < fileTextureCount){
                            for(int j = 0; j < fileTextureCount; ++j){
                                FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>(j);
                                if(texture){
                                    std::string textureName = texture->GetRelativeFileName();
                                    materials[i].textureName.push_back(textureName);
                                    std::string UVSetName = texture->UVSet.Get().Buffer();
                                    std::cout << textureName << std::endl;
                                    std::cout << UVSetName << std::endl;
                                }
                            }
                        }
                    }
                }
            }
            
            // attribute取得
            const bool hasColor = mesh->GetElementVertexColorCount() > 0;
            const bool hasNormal = mesh->GetElementNormalCount() > 0;
            const bool hasTexCoord = mesh->GetElementUVCount() > 0;
            const bool hasTangent = mesh->GetElementTangentCount() > 0;
            
            FbxStringList uvSetNameList;
            mesh->GetUVSetNames(uvSetNameList);
            
            FbxGeometryElement::EMappingMode colorMappingMode = FbxGeometryElement::EMappingMode::eNone;
            bool isColorDirectRef = true;
            const FbxLayerElementArrayTemplate<int>* colorIndexList = nullptr;
            const FbxLayerElementArrayTemplate<FbxColor>* colorList = nullptr;
            if(hasColor){
                const FbxGeometryElementVertexColor* fbxColorList = mesh->GetElementVertexColor();
                colorMappingMode = fbxColorList->GetMappingMode();
                isColorDirectRef = fbxColorList->GetReferenceMode() == FbxLayerElement::eDirect;
                colorIndexList = &fbxColorList->GetIndexArray();
                colorList = &fbxColorList->GetDirectArray();
            }
            
            FbxGeometryElement::EMappingMode tangentMappingMode = FbxGeometryElement::EMappingMode::eNone;
            bool isTangentDirectRef = true;
            const FbxLayerElementArrayTemplate<int>* tangentIndexList = nullptr;
            const FbxLayerElementArrayTemplate<FbxVector4>* tangentList = nullptr;
            FbxGeometryElement::EMappingMode binormalMappingMode = FbxGeometryElement::EMappingMode::eNone;
            bool isBinormalDirectRef = true;
            const FbxLayerElementArrayTemplate<int>* binormalIndexList = nullptr;
            const FbxLayerElementArrayTemplate<FbxVector4>* binormalList = nullptr;
            if(hasTangent){
                const FbxGeometryElementTangent* fbxTangentList = mesh->GetElementTangent();
                tangentMappingMode = fbxTangentList->GetMappingMode();
                isTangentDirectRef = fbxTangentList->GetReferenceMode() == FbxLayerElement::eDirect;
                tangentIndexList = &fbxTangentList->GetIndexArray();
                tangentList = &fbxTangentList->GetDirectArray();
                const FbxGeometryElementBinormal* fbxBinormalList = mesh->GetElementBinormal();
                binormalMappingMode = fbxBinormalList->GetMappingMode();
                isBinormalDirectRef = fbxBinormalList->GetReferenceMode() == FbxLayerElement::eDirect;
                binormalIndexList = &fbxBinormalList->GetIndexArray();
                binormalList = &fbxBinormalList->GetDirectArray();
            }
            
            const FbxLayerElementArrayTemplate<int>* materialIndexList = nullptr;
            if(FbxGeometryElementMaterial* fbxMaterialLayer = mesh->GetElementMaterial()){
                materialIndexList = &fbxMaterialLayer->GetIndexArray();
            }
            
            //最適化用
            //[マテリアルカウント][cpIndex]
            std::vector<std::vector<Relation>> relations(materialCount);
            for(auto& rel : relations){
                rel.resize(cpCount);
            }
            
            const int polygonCount = mesh->GetPolygonCount();
            
            int polygonVertex = 0;
            for(int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex){
                for(int pos = 0; pos < 3; ++pos){
                    const int cpIndex = mesh->GetPolygonVertex(polygonIndex, pos);
                    SkinnedVertex v;
                    v.position = toVector3(mat.MultT(controlPoints[cpIndex]));
                    static float buf = 10.0f;
                    static int cou = 0;
                    cou++;
                    
                    if(hasColor){
                        v.color  = toVector4(GetElement(colorMappingMode, isColorDirectRef, colorIndexList, colorList, cpIndex, polygonVertex,FbxColor(1,1,1,1)));
                    }
                    if(hasTexCoord){
                        FbxVector2 uv;
                        bool unmapped;
                        mesh->GetPolygonVertexUV(polygonIndex, pos, uvSetNameList[0], uv, unmapped);
                        v.texCoord = toVector2(uv);
                    }
                    
                    if(hasNormal){
                        FbxVector4 norm;
                        mesh->GetPolygonVertexNormal(polygonIndex, pos, norm);
                        v.normal = Normalize(toVector3(rot.MultT(norm)));
                    }
                    
                    v.tangent = Vector4(1,0,0,1);
                    if(hasTangent){
                        Vector3 binormal = toVector3
                        (rot.MultT(GetElement(binormalMappingMode, isBinormalDirectRef, binormalIndexList, binormalList,
                                              cpIndex, polygonVertex, FbxVector4(0,0,0,1))));
                        
                        Vector3 tangent = toVector3
                        (rot.MultT(GetElement(tangentMappingMode, isTangentDirectRef, tangentIndexList, tangentList,
                                              cpIndex, polygonVertex, FbxVector4(1,0,0,1))));
                        
                        v.tangent = Vector4(tangent, 1);
                        
                        Vector3 binormalTmp = Normalize(cross(v.normal, tangent));
                        if(dot(binormal,binormalTmp) < 0){
                            v.tangent.w = -1;
                        }
                    }
                    
                    v.color = Vector4((Vector3(1,1,1) + v.normal) * 0.5f);
                    
                    
                    //----------------------------------------------------------------------
                    //差分
                    
                    for(int boneIndex = 0; boneIndex < cpWeights[cpIndex].weights.size(); ++boneIndex){
                        v.boneIndex[boneIndex] = cpWeights[cpIndex].weights[boneIndex].first;
                        v.weights[boneIndex] = cpWeights[cpIndex].weights[boneIndex].second;
                    }
                    
                    //----------------------------------------------------------------------

                    //ポリゴンの所属するマテリアルのインデックスの取得
                    int materialIndex = materialIndexList ? (*materialIndexList)[polygonIndex] : 0;
                    Material<SkinnedVertex>& materialData = materials[materialIndex];
                    
                    int size = relations[materialIndex][cpIndex].relatedIndex.size();
                    if(size == 0){
                        int pushIndex = materialData.verteces.size();
                        materialData.indeces.push_back(pushIndex);
                        materialData.verteces.push_back(v);
                        relations[materialIndex][cpIndex].relatedIndex.push_back(pushIndex);
                    }
                    else {
                        bool isSame = false;
                        auto itr = relations[materialIndex][cpIndex].relatedIndex.begin();
                        for(;itr != relations[materialIndex][cpIndex].relatedIndex.end(); ++itr){
                            SkinnedVertex& tmp = materialData.verteces[*itr];
                            if(tmp.color == v.color &&
                               tmp.texCoord == v.texCoord &&
                               tmp.normal == v.normal){
                                isSame = true;
                                break;
                            }
                        }
                        if(isSame){
                            materialData.indeces.push_back(*itr);
                        }
                        else {
                            int pushIndex = materialData.verteces.size();
                            materialData.indeces.push_back(pushIndex);
                            materialData.verteces.push_back(v);
                            relations[materialIndex][cpIndex].relatedIndex.push_back(pushIndex);
                        }
                    }
                }
            }
        }
        
        
        /**
         * アニメーションをしないメッシュの読み込み
         *
         * @param   mesh    読み込むメッシュ
         * @param   meshRef 読み込んだデータの格納先
         */
        void FbxLoader::LoadStaticeMesh(fbxsdk::FbxMesh* mesh, StaticMesh& meshRef){
            auto toVector4 = [](auto v){
                return Vector4(v[0],v[1],v[2],v[3]);
            };
            
            auto toVector3 = [](auto v){
                return Vector3(v[0],v[1],v[2]);
            };
            
            auto toVector2 = [](FbxVector2 v){
                return Vector2(v[0],v[1]);
            };
            
            //頂点数
            int cpCount = mesh->GetControlPointsCount();
            //頂点配列
            FbxVector4* controlPoints = mesh->GetControlPoints();
            
            FbxNode* meshNode = mesh->GetNode();
            
            FbxAMatrix mat = meshNode->EvaluateGlobalTransform();
            FbxAMatrix rot(FbxVector4(0, 0, 0), mat.GetR(), FbxVector4(1,1,1));
            
            int materialCount = mesh->GetNode()->GetMaterialCount();
            std::cout << "material num : " << materialCount << std::endl;
            materialCount = materialCount ? materialCount : 1;
            
            auto& materials = meshRef.materials;
            materials.resize(materialCount);
            
            //テクスチャ取得
            for(int i = 0; i < materialCount; ++i){
                FbxSurfaceMaterial* material = meshNode->GetMaterial(i);
                if(material){
                    materials[i].name = material->GetName();
                    FbxProperty property = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
                    int layerNum = property.GetSrcObjectCount<FbxLayeredTexture>();
                    if(0 < layerNum){
                        for(int j = 0; j < layerNum; ++j){
                            FbxLayeredTexture* layeredTexture = property.GetSrcObject<FbxLayeredTexture>(j);
                            int textureCount = layeredTexture->GetSrcObjectCount<FbxFileTexture>();
                            for(int textureIndex = 0; textureIndex < textureCount; ++textureIndex){
                                FbxFileTexture* texture = layeredTexture->GetSrcObject<FbxFileTexture>(textureIndex);
                                if(texture){
                                    std::string textureName = texture->GetRelativeFileName();
                                    std::cout << textureName << std::endl;
                                    materials[i].textureName.push_back(textureName);
                                }
                            }
                        }
                    }
                    else {
                        int fileTextureCount = property.GetSrcObjectCount<FbxFileTexture>();
                        if(0 < fileTextureCount){
                            for(int j = 0; j < fileTextureCount; ++j){
                                FbxFileTexture* texture = property.GetSrcObject<FbxFileTexture>(j);
                                if(texture){
                                    std::string textureName = texture->GetRelativeFileName();
                                    materials[i].textureName.push_back(textureName);
                                    std::string UVSetName = texture->UVSet.Get().Buffer();
                                    std::cout << textureName << std::endl;
                                    std::cout << UVSetName << std::endl;
                                }
                            }
                        }
                    }
                }
            }
            
            // attribute取得
            const bool hasColor = mesh->GetElementVertexColorCount() > 0;
            const bool hasNormal = mesh->GetElementNormalCount() > 0;
            const bool hasTexCoord = mesh->GetElementUVCount() > 0;
            const bool hasTangent = mesh->GetElementTangentCount() > 0;
            
            FbxStringList uvSetNameList;
            mesh->GetUVSetNames(uvSetNameList);
            
            FbxGeometryElement::EMappingMode colorMappingMode = FbxGeometryElement::EMappingMode::eNone;
            bool isColorDirectRef = true;
            const FbxLayerElementArrayTemplate<int>* colorIndexList = nullptr;
            const FbxLayerElementArrayTemplate<FbxColor>* colorList = nullptr;
            if(hasColor){
                const FbxGeometryElementVertexColor* fbxColorList = mesh->GetElementVertexColor();
                colorMappingMode = fbxColorList->GetMappingMode();
                isColorDirectRef = fbxColorList->GetReferenceMode() == FbxLayerElement::eDirect;
                colorIndexList = &fbxColorList->GetIndexArray();
                colorList = &fbxColorList->GetDirectArray();
            }
            
            FbxGeometryElement::EMappingMode tangentMappingMode = FbxGeometryElement::EMappingMode::eNone;
            bool isTangentDirectRef = true;
            const FbxLayerElementArrayTemplate<int>* tangentIndexList = nullptr;
            const FbxLayerElementArrayTemplate<FbxVector4>* tangentList = nullptr;
            FbxGeometryElement::EMappingMode binormalMappingMode = FbxGeometryElement::EMappingMode::eNone;
            bool isBinormalDirectRef = true;
            const FbxLayerElementArrayTemplate<int>* binormalIndexList = nullptr;
            const FbxLayerElementArrayTemplate<FbxVector4>* binormalList = nullptr;
            if(hasTangent){
                const FbxGeometryElementTangent* fbxTangentList = mesh->GetElementTangent();
                tangentMappingMode = fbxTangentList->GetMappingMode();
                isTangentDirectRef = fbxTangentList->GetReferenceMode() == FbxLayerElement::eDirect;
                tangentIndexList = &fbxTangentList->GetIndexArray();
                tangentList = &fbxTangentList->GetDirectArray();
                const FbxGeometryElementBinormal* fbxBinormalList = mesh->GetElementBinormal();
                binormalMappingMode = fbxBinormalList->GetMappingMode();
                isBinormalDirectRef = fbxBinormalList->GetReferenceMode() == FbxLayerElement::eDirect;
                binormalIndexList = &fbxBinormalList->GetIndexArray();
                binormalList = &fbxBinormalList->GetDirectArray();
            }
            
            const FbxLayerElementArrayTemplate<int>* materialIndexList = nullptr;
            if(FbxGeometryElementMaterial* fbxMaterialLayer = mesh->GetElementMaterial()){
                materialIndexList = &fbxMaterialLayer->GetIndexArray();
            }
            
            //最適化用
            //[マテリアルカウント][cpIndex]
            std::vector<std::vector<Relation>> relations(materialCount);
            for(auto& rel : relations){
                rel.resize(cpCount);
            }
            
            const int polygonCount = mesh->GetPolygonCount();
            
            int polygonVertex = 0;
            for(int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex){
                for(int pos = 0; pos < 3; ++pos){
                    
                    const int cpIndex = mesh->GetPolygonVertex(polygonIndex, pos);
                    StaticVertex v;
                    v.position = toVector3(mat.MultT(controlPoints[cpIndex]));
                    static float buf = 10.0f;
                    static int cou = 0;
                    cou++;
                    
                    if(hasColor){
                        v.color  = toVector4(GetElement(colorMappingMode, isColorDirectRef, colorIndexList, colorList, cpIndex, polygonVertex,FbxColor(1,1,1,1)));
                    }
                    if(hasTexCoord){
                        FbxVector2 uv;
                        bool unmapped;
                        mesh->GetPolygonVertexUV(polygonIndex, pos, uvSetNameList[0], uv, unmapped);
                        v.texCoord = toVector2(uv);
                    }
                    
                    if(hasNormal){
                        FbxVector4 norm;
                        mesh->GetPolygonVertexNormal(polygonIndex, pos, norm);
                        v.normal = Normalize(toVector3(rot.MultT(norm)));
                    }
                    
                    v.tangent = Vector4(1,0,0,1);
                    if(hasTangent){
                        Vector3 binormal = toVector3
                        (rot.MultT(GetElement(binormalMappingMode, isBinormalDirectRef, binormalIndexList, binormalList,
                                              cpIndex, polygonVertex, FbxVector4(0,0,0,1))));
                        
                        Vector3 tangent = toVector3
                        (rot.MultT(GetElement(tangentMappingMode, isTangentDirectRef, tangentIndexList, tangentList,
                                              cpIndex, polygonVertex, FbxVector4(1,0,0,1))));
                        
                        v.tangent = Vector4(tangent, 1);
                        
                        Vector3 binormalTmp = Normalize(cross(v.normal, tangent));
                        if(dot(binormal,binormalTmp) < 0){
                            v.tangent.w = -1;
                        }
                    }
                    
                    v.color = Vector4((Vector3(1,1,1) + v.normal) * 0.5f);
                    
                    //ポリゴンの所属するマテリアルのインデックスの取得
                    int materialIndex = materialIndexList ? (*materialIndexList)[polygonIndex] : 0;
                    Material<StaticVertex>& materialData = materials[materialIndex];
                    
                    int size = relations[materialIndex][cpIndex].relatedIndex.size();
                    if(size == 0){
                        int pushIndex = materialData.verteces.size();
                        materialData.indeces.push_back(pushIndex);
                        materialData.verteces.push_back(v);
                        relations[materialIndex][cpIndex].relatedIndex.push_back(pushIndex);
                    }
                    else {
                        bool isSame = false;
                        auto itr = relations[materialIndex][cpIndex].relatedIndex.begin();
                        for(;itr != relations[materialIndex][cpIndex].relatedIndex.end(); ++itr){
                            StaticVertex& tmp = materialData.verteces[*itr];
                            if(tmp.color == v.color &&
                               tmp.texCoord == v.texCoord &&
                               tmp.normal == v.normal){
                                isSame = true;
                                break;
                            }
                        }
                        if(isSame){
                            materialData.indeces.push_back(*itr);
                        }
                        else {
                            int pushIndex = materialData.verteces.size();
                            materialData.indeces.push_back(pushIndex);
                            materialData.verteces.push_back(v);
                            relations[materialIndex][cpIndex].relatedIndex.push_back(pushIndex);
                        }
                    }
                }
            }
        }
        
        /**
         * アニメーションデータの読み込み
         *
         * @param   animations  アニメーションデータの格納先
         */
        void FbxLoader::LoadAnimation(std::vector<Animation>& animations){
            //アニメーション数の取得
            int animCount = pScene->GetSrcObjectCount<FbxAnimStack>();
            FbxTime period;
            period.SetTime(0, 0, 0, 1, 0, pScene->GetGlobalSettings().GetTimeMode());
            
            //            auto toMyQuat = [](FbxQuaternion fbxquat){
            //                return Quaternion(fbxquat[0],fbxquat[1],fbxquat[2],fbxquat[3]);
            //            };
            //
            //            auto toVector3 = [](auto v){
            //                return Vector3(v[0],v[1],v[2]);
            //            };
            
            auto GetGeometry = [](FbxNode* pNode)->FbxAMatrix
            {
                const FbxVector4 lT = pNode->GetGeometricTranslation(FbxNode::eSourcePivot);
                const FbxVector4 lR = pNode->GetGeometricRotation(FbxNode::eSourcePivot);
                const FbxVector4 lS = pNode->GetGeometricScaling(FbxNode::eSourcePivot);
                
                return FbxAMatrix(lT, lR, lS);
            };
            
            animations.resize(animCount);
            for(int animIndex = 0; animIndex < animCount; ++animIndex){
                FbxAnimStack* animStack = pScene->GetSrcObject<FbxAnimStack>(animIndex);
                pScene->SetCurrentAnimationStack(animStack);
                std::string animName = animStack->GetName();
                FbxTime start = animStack->GetLocalTimeSpan().GetStart();
                FbxTime stop = animStack->GetLocalTimeSpan().GetStop();
                int keyCount = animStack->GetLocalTimeSpan().GetDuration().Get() / period.Get();
                
                animStack->GetReferenceTimeSpan();
                
                animations[animIndex].animationTime = stop.GetSecondDouble();
                
                animations[animIndex].boneAnimationData.resize(publicBoneTree.data.size());
                
                int clusterCount = pScene->GetSrcObjectCount<FbxCluster>();
                for(int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex){
                    FbxCluster* cluster = pScene->GetSrcObject<FbxCluster>(clusterIndex);
                    auto includeMesh = FindIncludedMesh(cluster);
                    
                    FbxNode* meshNode = nullptr;
                    if(includeMesh){
                        meshNode = includeMesh->GetNode();
                    }
                    auto pData = publicBoneTree.FindBone(cluster->GetLink()->GetName());
                    if(!pData){
                        continue;
                    }
                    
                    auto& buf = animations[animIndex].boneAnimationData[pData->boneId];
                    for(int keyframe = 0; keyframe <= keyCount; ++keyframe){
                        FbxAMatrix mat = cluster->GetLink()->EvaluateGlobalTransform(period * keyframe);
                        if(meshNode && !boneBaseGetFromLink){
                            FbxAMatrix meshMat = meshNode->EvaluateGlobalTransform(period * keyframe);
                            
                            mat = (meshMat * GetGeometry(meshNode)).Inverse() * mat;
                        }
                        buf.animDatas.push_back
                        ({
                            static_cast<float>((period * (keyframe)).GetSecondDouble()),
                            toMyMat(mat)
                        });
                    }
                    buf.animationTime = buf.animDatas.back().first;
                }
            }
        }
    }// namespace FbxLoader
}// namespace myTools


