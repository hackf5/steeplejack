// spell-checker: ignore MSAA CHECKVERSION

#include "gui.h"

#include "spdlog/spdlog.h"

#include <array>
#include <stdexcept>

using namespace steeplejack;

Gui::Gui(const Window& window, const Device& device, const RenderPass& render_pass) :
    m_device(device), m_descriptor_pool(create_descriptor_pool()), m_framerate()
{
    spdlog::info("Creating GUI");

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForVulkan(window.glfw(), true);

    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = device.instance();
    init_info.PhysicalDevice = device.physical_device();
    init_info.Device = device.vk();
    init_info.QueueFamily = device.graphics_queue_index();
    init_info.Queue = device.graphics_queue();
    init_info.RenderPass = render_pass.vk();
    init_info.DescriptorPool = m_descriptor_pool;

    init_info.PipelineCache = nullptr;
    init_info.Allocator = nullptr;

    init_info.MinImageCount = Device::kMaxFramesInFlight;
    init_info.ImageCount = Device::kMaxFramesInFlight;
    init_info.CheckVkResultFn = check_vk_result;
    init_info.MSAASamples = device.msaa_samples();

    ImGui_ImplVulkan_Init(&init_info);

    ImGui_ImplVulkan_CreateFontsTexture();
}

VkDescriptorPool Gui::create_descriptor_pool()
{
    spdlog::info("Creating GUI Descriptor Pool");

    const std::array<VkDescriptorPoolSize, 1> pool_sizes{
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1},
    };
    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
    pool_info.pPoolSizes = pool_sizes.data();

    VkDescriptorPool descriptor_pool = nullptr;
    if (vkCreateDescriptorPool(m_device.vk(), &pool_info, nullptr, &descriptor_pool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create descriptor pool");
    }

    return descriptor_pool;
}

Gui::~Gui()
{
    spdlog::info("Destroying GUI");

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    vkDestroyDescriptorPool(m_device.vk(), m_descriptor_pool, nullptr);
}

void Gui::check_vk_result(VkResult result)
{
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("ImGui_ImplVulkan_Init failed");
    }
}

void Gui::begin_frame()
{
    m_framerate.next_frame();

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Info", nullptr, ImGuiWindowFlags_None);
    ImGui::Text("FPS: %d", m_framerate.fps());
    ImGui::End();
}

void Gui::render(VkCommandBuffer command_buffer)
{
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), command_buffer);
}