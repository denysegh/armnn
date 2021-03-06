//
// Copyright © 2017 Arm Ltd. All rights reserved.
// SPDX-License-Identifier: MIT
//

#include "ClBackend.hpp"
#include "ClBackendId.hpp"
#include "ClWorkloadFactory.hpp"
#include "ClBackendContext.hpp"
#include "ClLayerSupport.hpp"
#include "ClTensorHandleFactory.hpp"

#include <armnn/BackendRegistry.hpp>

#include <aclCommon/BaseMemoryManager.hpp>

#include <armnn/backends/IBackendContext.hpp>
#include <armnn/backends/IMemoryManager.hpp>

#include <armnn/utility/PolymorphicDowncast.hpp>

#include <Optimizer.hpp>

#include <arm_compute/runtime/CL/CLBufferAllocator.h>

namespace armnn
{

const BackendId& ClBackend::GetIdStatic()
{
    static const BackendId s_Id{ClBackendId()};
    return s_Id;
}

IBackendInternal::IMemoryManagerUniquePtr ClBackend::CreateMemoryManager() const
{
    return std::make_unique<ClMemoryManager>(std::make_unique<arm_compute::CLBufferAllocator>());
}

IBackendInternal::IWorkloadFactoryPtr ClBackend::CreateWorkloadFactory(
    const IBackendInternal::IMemoryManagerSharedPtr& memoryManager) const
{
    return std::make_unique<ClWorkloadFactory>(
        PolymorphicPointerDowncast<ClMemoryManager>(memoryManager));
}

IBackendInternal::IWorkloadFactoryPtr ClBackend::CreateWorkloadFactory(
    TensorHandleFactoryRegistry& registry) const
{
    auto memoryManager = std::make_shared<ClMemoryManager>(std::make_unique<arm_compute::CLBufferAllocator>());

    registry.RegisterMemoryManager(memoryManager);
    registry.RegisterFactory(std::make_unique<ClTensorHandleFactory>(memoryManager));

    return std::make_unique<ClWorkloadFactory>(
            PolymorphicPointerDowncast<ClMemoryManager>(memoryManager));
}

std::vector<ITensorHandleFactory::FactoryId> ClBackend::GetHandleFactoryPreferences() const
{
    return std::vector<ITensorHandleFactory::FactoryId> {ClTensorHandleFactory::GetIdStatic()};
}

void ClBackend::RegisterTensorHandleFactories(TensorHandleFactoryRegistry& registry)
{
    auto mgr = std::make_shared<ClMemoryManager>(std::make_unique<arm_compute::CLBufferAllocator>());

    registry.RegisterMemoryManager(mgr);
    registry.RegisterFactory(std::make_unique<ClTensorHandleFactory>(mgr));
}

IBackendInternal::IBackendContextPtr
ClBackend::CreateBackendContext(const IRuntime::CreationOptions& options) const
{
    return IBackendContextPtr{new ClBackendContext{options}};
}

IBackendInternal::IBackendProfilingContextPtr ClBackend::CreateBackendProfilingContext(
    const IRuntime::CreationOptions&, IBackendProfilingPtr&)
{
    return IBackendProfilingContextPtr{};
}

IBackendInternal::Optimizations ClBackend::GetOptimizations() const
{
    return Optimizations{};
}

IBackendInternal::ILayerSupportSharedPtr ClBackend::GetLayerSupport() const
{
    static ILayerSupportSharedPtr layerSupport{new ClLayerSupport};
    return layerSupport;
}

OptimizationViews ClBackend::OptimizeSubgraphView(const SubgraphView& subgraph) const
{
    OptimizationViews optimizationViews;

    optimizationViews.AddUntouchedSubgraph(SubgraphView(subgraph));

    return optimizationViews;
}

} // namespace armnn
