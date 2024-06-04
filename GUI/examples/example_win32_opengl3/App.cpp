#include "App.h"
#include "imgui.h"
#include "OrderBook\Book.h"
#include "OrderBook\Limit.h"
#include "OrderBook\Order.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace MyApp
{
    static std::unordered_map<std::string, std::shared_ptr<Order>> orders;
    static std::shared_ptr<Book> orderBook = std::make_shared<Book>();
    static std::string logBuffer;
    struct TableRow {
        std::optional<int> bidSize;
        std::optional<int> bidVolume;
        std::optional<float> bidPrice;
        std::optional<float> offerPrice;
        std::optional<int> offerVolume;
        std::optional<int> offerSize;
    };
    static std::vector<TableRow> orderBookData;

    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm now_tm = *std::localtime(&now_time);

        std::ostringstream oss;
        oss << std::put_time(&now_tm, "%Y-%m-%d %H:%M:%S");

        return "[" + oss.str() + "]";
    }

    void inOrder(std::shared_ptr<Limit> root, std::vector<std::shared_ptr<Limit>>& vec)
    {
        if (root == nullptr) {
            return;
        }
        inOrder(root->getLeftChild(), vec); // Traverse the left subtree
        vec.emplace_back(root);
        inOrder(root->getRightChild(), vec); // Traverse the right subtree
    }
    void reverseInOrder(std::shared_ptr<Limit> root, std::vector<std::shared_ptr<Limit>>& vec)
    {
        if (root == nullptr) {
            return;
        }
        reverseInOrder(root->getRightChild(), vec); // Traverse the right subtree
        vec.emplace_back(root);
        reverseInOrder(root->getLeftChild(), vec); // Traverse the left subtree
    }
    void updateOrderBookData()
    {
        orderBookData.clear();
        std::vector<std::shared_ptr<Limit>> asks;
        std::vector<std::shared_ptr<Limit>> bids;
        reverseInOrder(orderBook->getSellTree(), asks);
        reverseInOrder(orderBook->getBuyTree(), bids);

        int askIdx = 0, bidIdx = 0;

        while (askIdx < asks.size() || bidIdx < bids.size())
        {
            if (askIdx >= asks.size() && bidIdx < bids.size())
            {
                orderBookData.emplace_back(TableRow{ bids[bidIdx]->getSize(),
                    bids[bidIdx]->getTotalVol(),
                    bids[bidIdx]->getLimitPrice(),
                    std::nullopt,
                    std::nullopt,
                    std::nullopt });
                bidIdx++;
            }
            else if (askIdx < asks.size() && bidIdx >= bids.size())
            {
                orderBookData.emplace_back(TableRow{ std::nullopt,
                    std::nullopt,
                    std::nullopt,
                    asks[askIdx]->getLimitPrice(),
                    asks[askIdx]->getTotalVol(),
                    asks[askIdx]->getSize() });
                askIdx++;
            }
            else
            {
                if (asks[askIdx]->getLimitPrice() < bids[bidIdx]->getLimitPrice())
                {
                    orderBookData.emplace_back(TableRow{ bids[bidIdx]->getSize(),
                        bids[bidIdx]->getTotalVol(),
                        bids[bidIdx]->getLimitPrice(),
                        std::nullopt,
                        std::nullopt,
                        std::nullopt });
                    bidIdx++;
                }
                else if (asks[askIdx]->getLimitPrice() > bids[bidIdx]->getLimitPrice())
                {
                    orderBookData.emplace_back(TableRow{ std::nullopt,
                        std::nullopt,
                        std::nullopt,
                        asks[askIdx]->getLimitPrice(),
                        asks[askIdx]->getTotalVol(),
                        asks[askIdx]->getSize() });
                    askIdx++;
                }
                else
                {
                    orderBookData.emplace_back(TableRow{ bids[bidIdx]->getSize(),
                        bids[bidIdx]->getTotalVol(),
                        bids[bidIdx]->getLimitPrice(),
                        asks[askIdx]->getLimitPrice(),
                        asks[askIdx]->getTotalVol(),
                        asks[askIdx]->getSize() });
                    askIdx++;
                    bidIdx++;
                }
            }
        }
    }

    void RenderUI()
    {
        ImGui::Begin("Order Book");
        ImGui::SeparatorText("Select Action");
        static int action = 0;
        ImGui::RadioButton("Add Order", &action, 0); ImGui::SameLine();
        ImGui::RadioButton("Execute Order", &action, 1); ImGui::SameLine();
        ImGui::RadioButton("Cancel order", &action, 2);
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        if (action == 0) ImGui::BeginDisabled();
        ImGui::SeparatorText("Enter Order ID");
        static char orderIdBuffer[37] = "";
        ImGui::Text("Order ID:");
        ImGui::InputText("##Order ID:", orderIdBuffer, IM_ARRAYSIZE(orderIdBuffer));
        if (action == 0) ImGui::EndDisabled();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        if (action != 0) ImGui::BeginDisabled();
        ImGui::SeparatorText("Parameters");
        static int isBuy = 1;
        ImGui::RadioButton("Buy", &isBuy, 1); ImGui::SameLine();
        ImGui::RadioButton("Sell", &isBuy, 0);
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("No. of Shares:");
        static int numShares = 0;
        ImGui::InputInt("##No. of Shares:", &numShares);
        if (numShares < 0) numShares = 0;
        ImGui::Dummy(ImVec2(0.0f, 5.0f));
        ImGui::Text("Limit Price ($):");
        static float limitPrice = 0.0;
        ImGui::InputFloat("##Limit Price ($):", &limitPrice);
        if (limitPrice < 0.0) limitPrice = 0.0;
        if (action != 0) ImGui::EndDisabled();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f); // Make button round
        if (ImGui::Button("Confirm")) {
            // cancel
            if (action == 2)
            {
                std::string orderId(orderIdBuffer);
                if (orders.find(orderId) != orders.end())
                {
                    bool success = orderBook->cancelOrder(orders[orderId]);
                    if (success)
                    {
                        orders.erase(orderId);
                    }
                    updateOrderBookData();

                }
                else
                {
                    logBuffer += getCurrentTimestamp() + " No order with this ID exists.\n";
                }

            }
            // execute
            else if (action == 1)
            {
                std::string orderId(orderIdBuffer);
                if (orders.find(orderId) != orders.end())
                {
                    bool success = orderBook->executeOrder(orders[orderId]);
                    if (success && orders[orderId]->getShares() == 0)
                    {
                        orders.erase(orderId);
                    }
                    else
                    {
                        logBuffer += getCurrentTimestamp() + " Couldn't execute entire order.\n";
                    }
                    updateOrderBookData();

                }
                else
                {
                    logBuffer += getCurrentTimestamp() + " No order with this ID exists.\n";
                }
            }
            // add
            else
            {
                std::shared_ptr<Order> temp;
                temp = orderBook->addOrderToBook(isBuy, numShares, limitPrice);
                if (temp)
                {
                    orders.insert({ temp->getId(), temp });
                    logBuffer += getCurrentTimestamp() + " Order ID: " + temp->getId() + "\n";
                }

                updateOrderBookData();

            }
        }
        ImGui::PopStyleVar();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        float maxHeight = 200.0f;
        ImGui::Text("Orders:");
        ImGui::BeginChild("TableRegion", ImVec2(0, maxHeight), true, ImGuiWindowFlags_HorizontalScrollbar);
        if (ImGui::BeginTable("Table", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY)) {
            // Set up columns
            ImGui::TableSetupColumn("BID SIZE");
            ImGui::TableSetupColumn("BID VOLUME");
            ImGui::TableSetupColumn("BID PRICE");
            ImGui::TableSetupColumn("OFFER PRICE");
            ImGui::TableSetupColumn("OFFER VOLUME");
            ImGui::TableSetupColumn("OFFER SIZE");
            ImGui::TableSetupScrollFreeze(0, 1);
            ImGui::TableHeadersRow();

            // Fill rows
            for (const auto& row : orderBookData) {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                if (row.bidSize) {
                    ImGui::Text("%d", *row.bidSize);
                }
                else {
                    ImGui::Text("");
                }

                ImGui::TableSetColumnIndex(1);
                if (row.bidVolume) {
                    ImGui::Text("%d", *row.bidVolume);
                }
                else {
                    ImGui::Text("");
                }

                ImGui::TableSetColumnIndex(2);
                if (row.bidPrice) {
                    ImGui::Text("%.2f", *row.bidPrice);
                }
                else {
                    ImGui::Text("");
                }

                ImGui::TableSetColumnIndex(3);
                if (row.offerPrice) {
                    ImGui::Text("%.2f", *row.offerPrice);
                }
                else {
                    ImGui::Text("");
                }

                ImGui::TableSetColumnIndex(4);
                if (row.offerVolume) {
                    ImGui::Text("%d", *row.offerVolume);
                }
                else {
                    ImGui::Text("");
                }

                ImGui::TableSetColumnIndex(5);
                if (row.offerSize) {
                    ImGui::Text("%d", *row.offerSize);
                }
                else {
                    ImGui::Text("");
                }
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();
        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("Log:");
        ImGui::InputTextMultiline("##LogBox", &logBuffer[0], logBuffer.size() + 1, ImVec2(0, 100), ImGuiInputTextFlags_ReadOnly);


        ImGui::End();
    }
}
